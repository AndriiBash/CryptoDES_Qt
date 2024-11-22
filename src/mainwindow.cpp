#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("Курсовий проект");
}

MainWindow::~MainWindow()
{
    delete ui;
}

// Функція для шифрування/дешифрування за допомогою DES
QByteArray MainWindow::processDES(const QByteArray &data, QByteArray key, bool encrypt)
{
    QByteArray processedData = data;

    // Переконуємось, що ключ має довжину 8 байт
    while (key.size() < 8) {
        key.append('\0');
    }
    key = key.mid(0, 8);

    ui->textEdit_3->append("Ключ (з доповненням, якщо потрібно): " + QString(key.toHex()));

    // Початкова перестановка
    processedData = initialPermutation(processedData);
    ui->textEdit_3->append("Після початкової перестановки: " + QString(processedData.toHex()));

    // Розділення блоку на половини
    QByteArray leftHalf = processedData.mid(0, 4);
    QByteArray rightHalf = processedData.mid(4, 4);

    // Генерація 16 раундових ключів
    QVector<QByteArray> roundKeys(16);
    QByteArray currentKey = key;
    for (int i = 0; i < 16; ++i) {
        roundKeys[i] = currentKey;
        currentKey = currentKey.mid(1) + currentKey.left(1); // Зсув ключа вліво
    }

    // Застосування раундів
    for (int round = 0; round < 16; ++round) {
        int currentRound = encrypt ? round : (15 - round); // Обратний порядок при дешифруванні
        QByteArray temp = rightHalf;

        QByteArray roundResult = feistelFunction(rightHalf, roundKeys[currentRound]);

        // XOR з лівою половиною
        for (int i = 0; i < leftHalf.size(); ++i) {
            rightHalf[i] = leftHalf[i] ^ roundResult[i];
        }
        leftHalf = temp;

        ui->textEdit_3->append("Раунд " + QString::number(round + 1) + ": Ліва половина: " + leftHalf.toHex() +
                               ", Права половина: " + rightHalf.toHex());
    }

    // Об'єднання половин у зворотному порядку
    processedData = rightHalf + leftHalf;

    // Завершальна перестановка
    processedData = finalPermutation(processedData);
    ui->textEdit_3->append("Після завершальної перестановки: " + QString(processedData.toHex()));

    return processedData;
}


// Початкова перестановка
QByteArray MainWindow::initialPermutation(const QByteArray &block)
{
    QByteArray permuted(block.size(), 0);
    QVector<int> permutationTable = {2, 6, 3, 1, 4, 8, 5, 7}; // Приклад перестановки
    for (int i = 0; i < block.size(); ++i) {
        permuted[i] = block[permutationTable[i % permutationTable.size()] - 1];
    }
    return permuted;
}

// Завершальна перестановка
QByteArray MainWindow::finalPermutation(const QByteArray &block)
{
    QByteArray permuted(block.size(), 0);
    QVector<int> permutationTable = {4, 1, 3, 5, 7, 2, 8, 6}; // Зворотна перестановка
    for (int i = 0; i < block.size(); ++i) {
        permuted[i] = block[permutationTable[i % permutationTable.size()] - 1];
    }
    return permuted;
}

// Функція Фейстеля
QByteArray MainWindow::feistelFunction(const QByteArray &halfBlock, const QByteArray &key)
{
    QByteArray expandedHalf = halfBlock;
    for (int i = 0; i < halfBlock.size(); ++i) {
        expandedHalf[i] = halfBlock[i] ^ key[i % key.size()]; // XOR з ключем
    }
    return expandedHalf;
}

// Кнопка шифрування
void MainWindow::on_pushButton_clicked()
{
    ui->textEdit_3->clear(); // Очищення поля логів

    QString text = ui->textEdit->toPlainText(); // Отримуємо текст із textEdit
    QString key = ui->lineEdit->text(); // Отримуємо ключ із lineEdit

    QByteArray textData = text.toUtf8();
    QByteArray paddedText = textData;

    // Додаємо padding до тексту, щоб його довжина була кратною 8
    while (paddedText.size() % 8 != 0) {
        paddedText.append('\0');
    }

    ui->textEdit_3->append("Вхідний текст (з падінгом): " + QString(paddedText.toHex()));

    // Шифруємо кожен блок по 8 байт
    QByteArray encrypted;
    for (int i = 0; i < paddedText.size(); i += 8) {
        QByteArray block = paddedText.mid(i, 8);
        QByteArray encryptedBlock = processDES(block, key.toUtf8(), true);
        encrypted.append(encryptedBlock);
    }

    ui->textEdit_2->setPlainText(encrypted.toHex()); // Виводимо зашифрований текст
}

// Кнопка дешифрування
void MainWindow::on_pushButton_2_clicked()
{
    ui->textEdit_3->clear(); // Очистка логів

    QString encryptedTextHex = ui->textEdit_2->toPlainText();
    QString key = ui->lineEdit->text();

    QByteArray encryptedData = QByteArray::fromHex(encryptedTextHex.toUtf8());

    // Дешифруємо кожен блок по 8 байт
    QByteArray decrypted;
    for (int i = 0; i < encryptedData.size(); i += 8) {
        QByteArray block = encryptedData.mid(i, 8);
        QByteArray decryptedBlock = processDES(block, key.toUtf8(), false);
        decrypted.append(decryptedBlock);
    }

    // Видаляємо нулі, додані для вирівнювання
    while (!decrypted.isEmpty() && decrypted.endsWith('\0')) {
        decrypted.chop(1);
    }

    ui->textEdit->setPlainText(QString::fromUtf8(decrypted)); // Виводимо результат
    ui->textEdit_3->append("Розшифрований текст: " + QString::fromUtf8(decrypted));
}
