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

    prefixTable.setHorizontalHeaderItem(0, new QStandardItem(QString(tr("Network Byte"))));
    prefixTable.setHorizontalHeaderItem(1, new QStandardItem(QString(tr("Prefix"))));
    prefixTable.setHorizontalHeaderItem(2, new QStandardItem(QString(tr("Private Key"))));
    prefixTable.setHorizontalHeaderItem(3, new QStandardItem(QString(tr("Public Key"))));
    prefixTable.setHorizontalHeaderItem(4, new QStandardItem(QString(tr("Status"))));
    tableOnProxy = false;
    ui->tableView->setModel(&prefixTable);
    ui->tableView->setAlternatingRowColors(true);
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableView->verticalHeader()->hide();
    ui->tableView->setSortingEnabled(true);
    ui->tableView->setColumnHidden(0, true); // Hide Network Byte
    ui->tableView->setColumnHidden(2, true); // Hide Private Key
    ui->tableView->setColumnHidden(3, true); // Hide Public Key
    ui->tableView->resizeColumnsToContents();
    ui->tableView->horizontalHeader()->resizeSection(0, 40);
    ui->tableView->horizontalHeader()->resizeSection(1, 100);
    ui->tableView->horizontalHeader()->resizeSection(2, 100);
    ui->tableView->horizontalHeader()->resizeSection(3, 100);
    ui->tableView->horizontalHeader()->setResizeMode(4, QHeaderView::Stretch);
    ui->tableView->setContextMenuPolicy(Qt::CustomContextMenu);

    QAction *sendToFactoryAction = new QAction(tr("Send to Factory"), this);
    QAction *removeAction = new QAction(tr("Remove"), this);
    copyPubkAction = new QAction(tr("Copy Public key"), this);
    copyPkAction = new QAction(tr("Copy Private key (Hex)"), this);
    copyPkBase58Action = new QAction(tr("Copy Private key (Base58)"), this);
    showSolveDialogAction = new QAction(tr("Solve"), this);
    copyBtcAdAction = new QAction(tr("Copy Bitcoin Address"), this);
    copyPrefixAction = new QAction(tr("Copy Prefix"), this);
    copyBtcAdAction->setVisible(false);
    copyPubkAction->setVisible(false);
    copyPkAction->setVisible(false);
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
        QModelIndexList selection = ui->tableView->selectionModel()->selectedRows(4);
        QString status, privKey, pubKey;
        if(!selection.isEmpty()) {
            status = selection.at(0).data(0).toString();
            pubKey = ui->tableView->selectionModel()->selectedRows(3).at(0).data(0).toString();
            privKey = ui->tableView->selectionModel()->selectedRows(2).at(0).data(0).toString();

            if (pubKey.isEmpty())
                copyPubkAction->setVisible(false);
            else
                copyPubkAction->setVisible(true);

            if(privKey.isEmpty())
                copyPkAction->setVisible(false);
            else
                copyPkAction->setVisible(true);

            if (status == tr("Solved")) {
                copyPrefixAction->setVisible(false);
                showSolveDialogAction->setVisible(false);
                copyBtcAdAction->setVisible(true);
                if (!privKey.isEmpty())
                    copyPkBase58Action->setVisible(true);
            } else {
                copyPrefixAction->setVisible(true);
                showSolveDialogAction->setVisible(true);
                copyBtcAdAction->setVisible(false);
                copyPkBase58Action->setVisible(false);
            }

            contextMenu->exec(QCursor::pos());
        }
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
        addRowToTable("00", sl.at(0), sl.at(1), sl.at(2), status);
    }
}

void MainWindow::on_txtSearch_textChanged(QString searchText)
{
    QCompleter *completer = new QCompleter(dataBase.getAllFields(), this);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    ui->txtSearch->setCompleter(completer);

    if (!searchText.isEmpty()) {
        proxyModel.setSourceModel(&prefixTable);
        proxyModel.setFilterKeyColumn(-1);
        proxyModel.setFilterFixedString(searchText);
        ui->tableView->setModel(&proxyModel);
        tableOnProxy = true;
    } else {
        ui->tableView->setModel(&prefixTable);
        tableOnProxy = false;
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

    file.write(dataBase.getEncryptedData(password));

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

    VanityDB tempDataBase;
    if (!tempDataBase.setEncryptedData(fileData, password)) {
        QMessageBox::critical(this, tr("Error"), tr("File is corrupted!\nMaybe password was wrong."));
        return;
    }

    for (int i=0; i<tempDataBase.getRecordCount(); i++) {

        QString networkByte = tempDataBase.getRecord(i, VanityDB::networkByte);
        QString prefix = tempDataBase.getRecord(i, VanityDB::prefix);
        QString privateKey = tempDataBase.getRecord(i, VanityDB::privateKey);
        QString publicKey = tempDataBase.getRecord(i, VanityDB::publicKey);

        bool status = false;
        bc.setPrivateKey(privateKey);
        if (bc.getBitcoinAddress().startsWith(prefix))
            status = true;
        addRowToTable(networkByte, prefix, privateKey, publicKey, status);
    }

    QMessageBox::information(this, tr("Success"), tr("Vanity database was successfully loaded."));

}

void MainWindow::copyPubkActionSlot() {
    copyDataFromTable(3);
}

void MainWindow::copyPkActionSlot() {
    copyDataFromTable(2);
}

void MainWindow::copyPrefixActionSlot() {
    copyDataFromTable(1);
}

void MainWindow::resetTxtStyleSheetsSlot() {
    QString defaultStyleSheet = "font: 12pt \"Consolas, Monospace\";";
    ui->txtPubKey->setStyleSheet(defaultStyleSheet);
    ui->txtPrivKey->setStyleSheet(defaultStyleSheet);
    ui->txtSolution->setStyleSheet(defaultStyleSheet);
    ui->txBitcoinAd->setStyleSheet(defaultStyleSheet);
}

void MainWindow::copyBtcAdActionSlot() {
    QModelIndexList selection = ui->tableView->selectionModel()->selectedRows(2);
    if(!selection.isEmpty()) {
        if (!selection.at(0).data(0).toString().isEmpty()) {
            bc.setPrivateKey(selection.at(0).data(0).toString());
        } else {
            selection = ui->tableView->selectionModel()->selectedRows(3);
            bc.setPublicKey(selection.at(0).data(0).toString());
        }
        QApplication::clipboard()->setText(bc.getBitcoinAddress());
    }
}

void MainWindow::copyPkBase58ActionSlot() {
    QModelIndexList selection = ui->tableView->selectionModel()->selectedRows(2);
    if(!selection.isEmpty()) {
        bc.setPrivateKey(selection.at(0).data(0).toString());
        QApplication::clipboard()->setText(bc.getPrivateKey(true));
    }
}

void MainWindow::solveActionSlot() {
    QString prefix = ui->tableView->selectionModel()->selectedRows(1).at(0).data(0).toString();
    slvDialog->setPrefPrivPub(prefix,
                           ui->tableView->selectionModel()->selectedRows(2).at(0).data(0).toString(),
                           ui->tableView->selectionModel()->selectedRows(3).at(0).data(0).toString());
    slvDialog->exec();
    QString finalPrivateKey = slvDialog->getPrivateKey();
    QString finalPublicKey = slvDialog->getPublicKey();
    if (!finalPrivateKey.isEmpty() || !finalPublicKey.isEmpty()) {
        prefixTable.removeRow(ui->tableView->selectionModel()->currentIndex().row());
        addRowToTable("00", prefix, finalPrivateKey, finalPublicKey, true);
    }
}

void MainWindow::sendToFactoryActionSlot() {
    QModelIndexList selection = ui->tableView->selectionModel()->selectedRows(2);
    if(!selection.isEmpty()) {
        QString privKey = selection.at(0).data(0).toString();
        QString pubKey = ui->tableView->selectionModel()->selectedRows(3).at(0).data(0).toString();
        ui->txtPrivKey->setPlainText(privKey);
        ui->txtPubKey->setPlainText(pubKey);
        ui->txtSolution->setPlainText("");
        ui->tabWidget->setCurrentIndex(1);
    }
}

void MainWindow::removeActionSlot() {
    QModelIndexList selection = ui->tableView->selectionModel()->selectedRows(1);
    if(!selection.isEmpty())
        if (QMessageBox::warning(this,
                                 tr("Remove address"),
                                 tr("Are you sure you want to remove '") + selection.at(0).data(0).toString() +
                                 tr("'?\nYour keys for this address will be removed too!"),
                                 QMessageBox::Cancel,
                                 QMessageBox::Ok) == QMessageBox::Ok) {
            dataBase.removeAllRecords(ui->tableView->selectionModel()->selectedRows(0).at(0).data(0).toString(),
                                      selection.at(0).data(0).toString(),
                                      ui->tableView->selectionModel()->selectedRows(2).at(0).data(0).toString(),
                                      ui->tableView->selectionModel()->selectedRows(3).at(0).data(0).toString());
            if (tableOnProxy)
                prefixTable.removeRow(proxyModel.mapToSource(ui->tableView->selectionModel()->currentIndex()).row());
            else
                prefixTable.removeRow(ui->tableView->selectionModel()->currentIndex().row());
        }
}

void MainWindow::copyDataFromTable(int column) {
    QModelIndexList selection = ui->tableView->selectionModel()->selectedRows(column);
    if(!selection.isEmpty())
        QApplication::clipboard()->setText(selection.at(0).data(0).toString());
}

void MainWindow::addRowToTable(QString networkByte, QString prefix, QString privatekey, QString publickey, bool solved) {
    QList<QStandardItem*> ql;
    ql.append(new QStandardItem(QString(networkByte)));
    ql.append(new QStandardItem(QString(prefix)));
    ql.append(new QStandardItem(QString(privatekey)));
    ql.append(new QStandardItem(QString(publickey)));
    if (solved)
        ql.append(new QStandardItem(QIcon(":/img/images/splitkey-solved.png"), QString(tr("Solved"))));
    else
        ql.append(new QStandardItem(QIcon(":/img/images/splitkey.png"), QString(tr("Unsolved"))));
    prefixTable.insertRow(0, ql);
    dataBase.addRecord(networkByte, prefix, privatekey, publickey);
}
