#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QRect position = frameGeometry();
    position.moveCenter(QDesktopWidget().availableGeometry().center());
    move(position.topLeft());

    prefixTable.setHorizontalHeaderItem(0, new QStandardItem(QString(tr("Prefix"))));
    prefixTable.setHorizontalHeaderItem(1, new QStandardItem(QString(tr("Private Key"))));
    prefixTable.setHorizontalHeaderItem(2, new QStandardItem(QString(tr("Status"))));
    ui->tableView->setModel(&prefixTable);
    ui->tableView->setAlternatingRowColors(true);
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableView->verticalHeader()->hide();
    ui->tableView->setSortingEnabled(true);
    ui->tableView->resizeColumnsToContents();
    ui->tableView->horizontalHeader()->resizeSection(0, 90);
    ui->tableView->horizontalHeader()->setResizeMode(1, QHeaderView::Stretch);
    ui->tableView->horizontalHeader()->resizeSection(2, 100);
    ui->tableView->setContextMenuPolicy(Qt::CustomContextMenu);

    QAction *copyPkAction = new QAction(tr("Copy Private key (Hex)"), this);
    QAction *sendToFactoryAction = new QAction(tr("Send to Factory"), this);
    QAction *removeAction = new QAction(tr("Remove"), this);
    QAction *copyPubkAction = new QAction(tr("Copy Public key"), this);
    copyPkBase58Action = new QAction(tr("Copy Private key (Base58)"), this);
    showSolveDialogAction = new QAction(tr("Solve"), this);
    copyBtcAdAction = new QAction(tr("Copy Bitcoin Address"), this);
    copyPrefixAction = new QAction(tr("Copy Prefix"), this);
    copyBtcAdAction->setVisible(false);
    copyPkBase58Action->setVisible(false);

    contextMenu = new QMenu();
    contextMenu->addAction(showSolveDialogAction);
    contextMenu->addAction(copyPrefixAction);
    contextMenu->addAction(copyBtcAdAction);
    contextMenu->addAction(copyPubkAction);
    contextMenu->addAction(sendToFactoryAction);
    contextMenu->addAction(copyPkAction);
    contextMenu->addAction(copyPkBase58Action);
    contextMenu->addAction(removeAction);

    connect(copyPkAction, SIGNAL(triggered()), this, SLOT(copyPkActionSlot()));
    connect(copyPkBase58Action, SIGNAL(triggered()), this, SLOT(copyPkBase58ActionSlot()));
    connect(sendToFactoryAction, SIGNAL(triggered()), this, SLOT(sendToFactoryActionSlot()));
    connect(removeAction, SIGNAL(triggered()), this, SLOT(removeActionSlot()));
    connect(copyPubkAction, SIGNAL(triggered()), this, SLOT(copyPubkActionSlot()));
    connect(showSolveDialogAction, SIGNAL(triggered()), this, SLOT(solveActionSlot()));
    connect(copyBtcAdAction, SIGNAL(triggered()), this, SLOT(copyBtcAdActionSlot()));
    connect(copyPrefixAction, SIGNAL(triggered()), this, SLOT(copyPrefixActionSlot()));

    npDialog = new NewPrefixDialog(this);
    slvDialog = new SolveDialog(this);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_txtSolution_textChanged()
{
    if (!bc.isHex(ui->txtSolution->toPlainText()))
        ui->txtSolution->setPlainText("");
}

void MainWindow::on_txtPrivKey_textChanged()
{
    if (!bc.isHex(ui->txtPrivKey->toPlainText()))
        ui->txtPrivKey->setPlainText("");
}

void MainWindow::on_txtPubKey_textChanged()
{
    if (!bc.isHex(ui->txtPubKey->toPlainText()))
        ui->txtPubKey->setPlainText("");
}

void MainWindow::on_tableView_customContextMenuRequested(QPoint pos)
{
    QModelIndex index = ui->tableView->indexAt(pos);
    if(index.isValid()) {
        QModelIndexList selection = ui->tableView->selectionModel()->selectedRows(2);
        if(!selection.isEmpty())
            if (selection.at(0).data(0).toString() == tr("Solved")) {
                copyPrefixAction->setVisible(false);
                showSolveDialogAction->setVisible(false);
                copyBtcAdAction->setVisible(true);
                copyPkBase58Action->setVisible(true);
            }
        contextMenu->exec(QCursor::pos());
        copyPrefixAction->setVisible(true);
        showSolveDialogAction->setVisible(true);
        copyBtcAdAction->setVisible(false);
        copyPkBase58Action->setVisible(false);
    }
}

void MainWindow::on_btnReGen_released()
{
    bc.generateKeyPairs();
    ui->txtPubKey->setPlainText(bc.getPublicKey());
    ui->txtPrivKey->setPlainText(bc.getPrivateKey());
}

void MainWindow::on_btnBitcoinAddress_released()
{
    QString changeStyleSheet = "font: 12pt \"Consolas, Monospace\"; background-color: #dddddd;";

    bc.setPrivateKey("");
    bc.setPublicKey("");
    ui->btnBitcoinAddress->setEnabled(false);

    ui->txBitcoinAd->setText("");
    if (ui->txtPrivKey->toPlainText().length() > 63) {
        bc.setPrivateKey(ui->txtPrivKey->toPlainText());
        if (ui->txtSolution->toPlainText().length() > 63) {
            if (ui->btnAddSol->isChecked())
                bc.mergeWithPrivateKey(ui->txtSolution->toPlainText());
            else
                bc.mergeWithPrivateKey(ui->txtSolution->toPlainText(), true);

            ui->txtSolution->setStyleSheet(changeStyleSheet);
            ui->txtSolution->setPlainText("");

            ui->txtPrivKey->setStyleSheet(changeStyleSheet);
            ui->txtPrivKey->setPlainText(bc.getPrivateKey());
        }
        ui->txtPubKey->setStyleSheet(changeStyleSheet);
        ui->txtPubKey->setPlainText(bc.getPublicKey());

    } else if (ui->txtPubKey->toPlainText().length() > 128) {
        bc.setPublicKey(ui->txtPubKey->toPlainText());
        if (ui->txtSolution->toPlainText().length() > 63) {
            if (ui->btnAddSol->isChecked())
                bc.mergeWithPublicKey(ui->txtSolution->toPlainText());
            else
                bc.mergeWithPublicKey(ui->txtSolution->toPlainText(), true);

            ui->txtSolution->setStyleSheet(changeStyleSheet); //#d0d0d0;
            ui->txtSolution->setPlainText("");

            ui->txtPubKey->setStyleSheet(changeStyleSheet); //#d5a4e3;
            ui->txtPubKey->setPlainText(bc.getPublicKey());
        }
    }

    if (bc.getPublicKey().length() > 128) {
        ui->txBitcoinAd->setStyleSheet(changeStyleSheet); //#fdef15
        ui->txBitcoinAd->setText(bc.getBitcoinAddress());
    }

    ui->btnBitcoinAddress->setEnabled(true);

    timer.start(1500);
    connect(&timer, SIGNAL(timeout()), this, SLOT(resetTxtStyleSheetsSlot()));
}

void MainWindow::on_btnNewPrefix_released()
{
    npDialog->exec();
    QStringList sl = npDialog->getPrefPrivPubValues();
    if (!sl.isEmpty()) {
        bool status = false;
        bc.setPublicKey(sl.at(2));
        if (bc.getBitcoinAddress().startsWith(sl.at(0)))
            status = true;
        addRowToTable(sl.at(0), sl.at(1), status);
    }
}

void MainWindow::on_btnSaveToFile_released()
{
    QString filename = QFileDialog::getSaveFileName(this,
                                                    tr("Save to file"),
                                                    QDesktopServices::storageLocation(QDesktopServices::DocumentsLocation),
                                                    tr("Vanity Datebase (*.vdb)"));
    if(filename == "") {
        QMessageBox::critical(this, tr("Error"), tr("No file name specified!"));
        return;
    }

    if (!filename.endsWith(".vdb"))
        filename += ".vdb";

    bool passwordOk = false;
    QString password = QInputDialog::getText(this, tr("Password"),
                                             tr("Enter a password to protect the database:"), QLineEdit::Password,
                                              "", &passwordOk);
    if (!passwordOk)
        return;
    else if (password.isEmpty()) {
        QMessageBox::critical(this, tr("Error"), tr("No password entered!"));
        return;
    }

    QFile file(filename);
    if(!file.open(QIODevice::WriteOnly)) {
        QMessageBox::critical(this, tr("Error"), tr("Cannot open file!"));
        return;
    }

    QString dbData = "";
    for(int i=0; i<prefixTable.rowCount(); ++i) {
        for(int j=0; j<prefixTable.columnCount()-1; ++j) {
            if(j!=0)
                dbData += ";";
            dbData += prefixTable.index(i,j).data(0).toString();
        }
        dbData += "\n";
    }

    QByteArray digest = QCryptographicHash::hash(dbData.toUtf8(), QCryptographicHash::Md5);

    // database version
    dbData += "\r$2$";

    // hash from table data for error detecting.
    // must be the last DB property.
    dbData += "\r$" + QString(digest.toHex()) + "$";

    // end sign of db
    dbData += "\r$\r$\r$";

    file.write(AESCrypt(dbData.toLocal8Bit(), password));

    file.close();

    QMessageBox::information(this, tr("Success"), tr("Address list was saved successfully."));
}

void MainWindow::on_btnLoadFromFile_released()
{
    QString filename = QFileDialog::getOpenFileName(this,
                                                    tr("Load from VDB file"),
                                                    QDesktopServices::storageLocation(QDesktopServices::DocumentsLocation),
                                                    tr("Vanity Datebase (*.vdb)"));

    if(filename == "") {
        QMessageBox::critical(this, tr("Error"), tr("No file name specified!"));
        return;
    }

    if (!filename.endsWith(".vdb"))
        filename += ".vdb";

    bool passwordOk = false;
    QString password = QInputDialog::getText(this, tr("Password"),
                                             tr("Enter password:"), QLineEdit::Password,
                                              "", &passwordOk);
    if (!passwordOk)
        return;
    else if (password.isEmpty()) {
        QMessageBox::critical(this, tr("Error"), tr("No password entered!"));
        return;
    }

    QFile file(filename);
    if(!file.open(QIODevice::ReadOnly)) {
        QMessageBox::critical(this, tr("Error"), tr("Cannot open file!"));
        return;
    }

    QByteArray fileData = file.readAll();
    file.close();

    QString dbData = QString(AESCrypt(fileData, password, true));

    dbData.truncate(dbData.indexOf("\r$\r$\r$"));

    QString tempData = dbData;
    tempData.truncate(dbData.indexOf("\r$"));

    QByteArray digest = QCryptographicHash::hash(tempData.toUtf8(), QCryptographicHash::Md5);
    if (!dbData.endsWith("$" + QString(digest.toHex()) + "$")) {
        QMessageBox::critical(this, tr("Error"), tr("File is corrupted!\nMaybe password was wrong."));
        return;
    }

    dbData = tempData;

    QStringList sl0 = dbData.split("\n");
    for (int i=0; i<sl0.length()-1; i++) {
        QStringList sl = sl0.at(i).split(";");
        bool status = false;
        bc.setPrivateKey(sl.at(1));
        if (bc.getBitcoinAddress().startsWith(sl.at(0)))
            status = true;
        addRowToTable(sl.at(0), sl.at(1), status);
    }

    QMessageBox::information(this, tr("Success"), tr("Vanity database was successfully loaded."));

}

void MainWindow::copyPkActionSlot() {
    copyDataFromTable(1);
}

void MainWindow::copyPrefixActionSlot() {
    copyDataFromTable(0);
}

void MainWindow::resetTxtStyleSheetsSlot() {
    QString defaultStyleSheet = "font: 12pt \"Consolas, Monospace\";";
    ui->txtPubKey->setStyleSheet(defaultStyleSheet);
    ui->txtPrivKey->setStyleSheet(defaultStyleSheet);
    ui->txtSolution->setStyleSheet(defaultStyleSheet);
    ui->txBitcoinAd->setStyleSheet(defaultStyleSheet);
}

void MainWindow::copyBtcAdActionSlot() {
    QModelIndexList selection = ui->tableView->selectionModel()->selectedRows(1);
    if(!selection.isEmpty()) {
        bc.setPrivateKey(selection.at(0).data(0).toString());
        QApplication::clipboard()->setText(bc.getBitcoinAddress());
    }
}

void MainWindow::copyPubkActionSlot() {
    QModelIndexList selection = ui->tableView->selectionModel()->selectedRows(1);
    if(!selection.isEmpty()) {
        bc.setPrivateKey(selection.at(0).data(0).toString());
        QApplication::clipboard()->setText(bc.getPublicKey());
    }
}

void MainWindow::copyPkBase58ActionSlot() {
    QModelIndexList selection = ui->tableView->selectionModel()->selectedRows(1);
    if(!selection.isEmpty()) {
        bc.setPrivateKey(selection.at(0).data(0).toString());
        QApplication::clipboard()->setText(bc.getPrivateKey(true));
    }
}

void MainWindow::solveActionSlot() {

    QString prefix = ui->tableView->selectionModel()->selectedRows(0).at(0).data(0).toString();
    slvDialog->setPrefPriv(prefix,
                           ui->tableView->selectionModel()->selectedRows(1).at(0).data(0).toString());
    slvDialog->exec();
    QString finalPrivateKey = slvDialog->getPrivateKey();
    if (!finalPrivateKey.isEmpty()) {
        prefixTable.removeRow(ui->tableView->selectionModel()->currentIndex().row());
        addRowToTable(prefix, finalPrivateKey, true);
    }
}

void MainWindow::sendToFactoryActionSlot() {
    QModelIndexList selection = ui->tableView->selectionModel()->selectedRows(1);
    if(!selection.isEmpty()) {
        QString pk = selection.at(0).data(0).toString();
        ui->txtPrivKey->setPlainText(pk);
        ui->txtPubKey->setPlainText("");
        ui->txtSolution->setPlainText("");
        ui->tabWidget->setCurrentIndex(1);
        on_btnBitcoinAddress_released();
    }
}

void MainWindow::removeActionSlot() {
    QModelIndexList selection = ui->tableView->selectionModel()->selectedRows(0);
    if(!selection.isEmpty())
        if (QMessageBox::warning(this,
                                 tr("Remove address"),
                                 tr("Are you sure you want to remove '") + selection.at(0).data(0).toString() +
                                 tr("'?\nYour private key for this address will be removed too!"),
                                 QMessageBox::Cancel,
                                 QMessageBox::Ok) == QMessageBox::Ok)
            prefixTable.removeRow(ui->tableView->selectionModel()->currentIndex().row());
}

void MainWindow::copyDataFromTable(int column) {
    QModelIndexList selection = ui->tableView->selectionModel()->selectedRows(column);
    if(!selection.isEmpty())
        QApplication::clipboard()->setText(selection.at(0).data(0).toString());
}

void MainWindow::addRowToTable(QString prefix, QString privatekey, bool solved) {
    QList<QStandardItem*> ql;
    ql.append(new QStandardItem(QString(prefix)));
    ql.append(new QStandardItem(QString(privatekey)));
    if (solved)
        ql.append(new QStandardItem(QIcon(":/img/images/splitkey-solved.png"), QString(tr("Solved"))));
    else
        ql.append(new QStandardItem(QIcon(":/img/images/splitkey.png"), QString(tr("Unsolved"))));
    prefixTable.insertRow(0, ql);
}

QByteArray MainWindow::AESCrypt(const QByteArray input, QString password, bool decrypt) {
    QByteArray output;
    const unsigned char ivector[] = "BVAMBVAMBVAMBVAM";
    unsigned char ckeyHash[256];
    int cLen = 0, fLen = 0;
    EVP_CIPHER_CTX cipherContext;


    EVP_CIPHER_CTX_init( &cipherContext );
    output.resize(input.length() + AES_BLOCK_SIZE + 100);
    SHA256(reinterpret_cast<const unsigned char*>(password.toStdString().c_str()), password.length(), ckeyHash);

    if (!decrypt) {
        EVP_EncryptInit_ex(&cipherContext,
                           EVP_aes_256_cbc(), NULL,
                           reinterpret_cast<const unsigned char*>(ckeyHash), ivector);

        EVP_EncryptUpdate(&cipherContext,
                          reinterpret_cast<unsigned char*>(output.data()),
                          &cLen,
                          reinterpret_cast<const unsigned char*>(input.constData()),
                          input.length());

        EVP_CipherFinal_ex(&cipherContext,
                           reinterpret_cast<unsigned char*>(output.data()) + cLen,
                           &fLen);

    } else {
        EVP_DecryptInit_ex(&cipherContext,
                           EVP_aes_256_cbc(), NULL,
                           reinterpret_cast<const unsigned char*>(ckeyHash), ivector);

        EVP_DecryptUpdate(&cipherContext,
                          reinterpret_cast<unsigned char*>(output.data()),
                          &cLen,
                          reinterpret_cast<const unsigned char*>(input.constData()),
                          input.length());

        EVP_DecryptFinal_ex(&cipherContext,
                           reinterpret_cast<unsigned char*>(output.data()) + cLen,
                           &fLen);

    }

    EVP_CIPHER_CTX_cleanup(&cipherContext);
    output.resize(cLen + fLen);

    return output;
}
