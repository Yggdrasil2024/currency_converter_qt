#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    // on ne peut pas agrandir
    setWindowFlags(windowFlags() & ~Qt::WindowMaximizeButtonHint);
    setFixedSize(width(), height());

    networkManager = new QNetworkAccessManager(this); // 'this' pour le parent
    // Connectez le signal finished() à un slot pour gérer la réponse
    connect(networkManager, &QNetworkAccessManager::finished,
            this, &MainWindow::onNetworkReplyFinished);
    connect(ui->convert, &QPushButton::clicked,
            this, &MainWindow::on_convert_clicked);
    connect(ui->refresh, &QPushButton::clicked,
            this, &MainWindow::on_refresh_clicked);

    //on met a jour les combos box
    combo_load_values();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::combo_load_values()
{
    qDebug() << "Début du chargement des devises...";

    // 1. Lecture du fichier currencies.json
    QFile file("currencies.json");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QString errorMsg = "Impossible d'ouvrir le fichier currencies.json : " + file.errorString();
        qDebug() << errorMsg;
        QMessageBox::warning(this, "Erreur de fichier", errorMsg);
        return;
    }

    // Lire tout le contenu du fichier
    QTextStream stream(&file);
    QString jsonString = stream.readAll();
    file.close();

    qDebug() << "Fichier currencies.json lu avec succès";

    // 2. Parsing JSON
    QJsonParseError parseError;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonString.toUtf8(), &parseError);

    if (parseError.error != QJsonParseError::NoError) {
        QString errorMsg = "Erreur de parsing JSON : " + parseError.errorString();
        qDebug() << errorMsg;
        QMessageBox::critical(this, "Erreur JSON", errorMsg);
        return;
    }

    if (!jsonDoc.isObject()) {
        QString errorMsg = "Le fichier JSON ne contient pas un objet valide";
        qDebug() << errorMsg;
        QMessageBox::critical(this, "Erreur JSON", errorMsg);
        return;
    }

    QJsonObject jsonObj = jsonDoc.object();
    qDebug() << "Parsing JSON réussi, nombre de devises trouvées :" << jsonObj.size();

    // 3. Extraction des devises et taux + stockage dans exchangeRates
    exchangeRates.clear(); // Vider la map existante
    QStringList currencyList; // Liste pour le tri alphabétique

    for (auto it = jsonObj.constBegin(); it != jsonObj.constEnd(); ++it) {
        QString currencyCode = it.key();
        double exchangeRate = it.value().toDouble();

        // Vérifier que la valeur est bien un nombre
        if (it.value().isDouble() || it.value().isString()) {
            exchangeRates[currencyCode] = exchangeRate;
            currencyList.append(currencyCode);
            qDebug() << "Devise ajoutée :" << currencyCode << "-> Taux :" << exchangeRate;
        } else {
            qDebug() << "Valeur invalide pour la devise" << currencyCode << ":" << it.value();
        }
    }

    // 4. Tri alphabétique des devises
    currencyList.sort();
    qDebug() << "Devises triées par ordre alphabétique";

    // 5. Mise à jour des QComboBox
    // Sauvegarder les sélections actuelles si elles existent
    QString currentSourceCurrency = ui->source_currency->currentText();
    QString currentTargetCurrency = ui->target_currency->currentText();

    // Effacer le contenu existant
    ui->source_currency->clear();
    ui->target_currency->clear();

    // Ajouter les devises triées
    ui->source_currency->addItems(currencyList);
    ui->target_currency->addItems(currencyList);

    // Restaurer les sélections précédentes si elles sont toujours valides
    int sourceIndex = ui->source_currency->findText(currentSourceCurrency);
    if (sourceIndex != -1) {
        ui->source_currency->setCurrentIndex(sourceIndex);
    }

    int targetIndex = ui->target_currency->findText(currentTargetCurrency);
    if (targetIndex != -1) {
        ui->target_currency->setCurrentIndex(targetIndex);
    }

    qDebug() << "ComboBox mises à jour avec" << currencyList.size() << "devises";

    // Optionnel : Définir des valeurs par défaut si aucune sélection précédente
    if (ui->source_currency->currentText().isEmpty() && currencyList.contains("USD")) {
        ui->source_currency->setCurrentText("USD");
    }
    if (ui->target_currency->currentText().isEmpty() && currencyList.contains("EUR")) {
        ui->target_currency->setCurrentText("EUR");
    }

    qDebug() << "Chargement des devises terminé avec succès";
}

void MainWindow::on_refresh_clicked()
{
    qDebug() << "Refresh button clicked. Fetching latest exchange rates...";

    // 1. Définir la devise de base pour la récupération des taux
    //    Choisissez une devise de base qui vous convient, par exemple USD.
    QString baseCurrency = "USD"; // Ou "EUR", "GBP", etc.

    // 2. Construire l'URL de l'API
    //    Assurez-vous que votre 'apiKey' est correctement défini dans mainwindow.h
    QString apiUrl = QString("https://v6.exchangerate-api.com/v6/%1/latest/%2")
                         .arg(apiKey) // Utilisez la variable membre apiKey
                         .arg(baseCurrency);

    QUrl url(apiUrl);

    if (!url.isValid()) {
        qDebug() << "Invalid API URL:" << apiUrl;
        // Gérer l'erreur, par exemple afficher un message à l'utilisateur
        return;
    }

    qDebug() << "Requesting URL:" << url.toString();

    // 3. Créer une requête réseau et l'envoyer
    QNetworkRequest request(url);
    networkManager->get(request);

    // Une fois la requête envoyée, la réponse sera traitée par le slot
    // onNetworkReplyFinished que nous avons connecté précédemment.
}

void MainWindow::onNetworkReplyFinished(QNetworkReply *reply)
{
    if (reply->error() == QNetworkReply::NoError) {
        QByteArray responseData = reply->readAll();
        qDebug() << "API Response received."; // Supprimez ou commentez ceci pour moins de spam console si la réponse est très longue

        // 1. Parser le JSON
        QJsonDocument jsonDoc = QJsonDocument::fromJson(responseData);

        if (jsonDoc.isNull()) {
            qDebug() << "Failed to create JSON document from response.";
            QMessageBox::warning(this, "Erreur API", "Impossible de parser la réponse JSON de l'API.");
            reply->deleteLater();
            return;
        }
        if (!jsonDoc.isObject()) {
            qDebug() << "JSON response is not an object.";
            QMessageBox::warning(this, "Erreur API", "La réponse JSON de l'API n'est pas un objet valide.");
            reply->deleteLater();
            return;
        }

        QJsonObject jsonObj = jsonDoc.object();

        // Vérifier le "result" de l'API pour s'assurer que la requête a réussi
        if (jsonObj.contains("result") && jsonObj["result"].toString() == "success") {
            qDebug() << "API call successful.";

            // 2. Extraire les taux de change
            //    L'API exchangeRate-api place les taux dans l'objet "conversion_rates"
            if (jsonObj.contains("conversion_rates") && jsonObj["conversion_rates"].isObject()) {
                QJsonObject conversionRates = jsonObj["conversion_rates"].toObject();

                // 3. Sauvegarder les données dans un fichier JSON local
                //    Nous allons sauvegarder l'intégralité de l'objet "conversion_rates" pour l'instant.
                //    Vous pourriez aussi sauvegarder toute la réponse si vous le souhaitez.
                QJsonDocument ratesDoc(conversionRates);

                QFile file(jsonFilePath); // Utilise le chemin du fichier déclaré en membre

                if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
                    file.write(ratesDoc.toJson(QJsonDocument::Indented)); // Indented pour une meilleure lisibilité
                    file.close();
                    qDebug() << "Exchange rates saved to" << jsonFilePath;
                    QMessageBox::information(this, "Mise à jour réussie", "Taux de change mis à jour avec succès !");

                    //mise a jour des combos box
                    combo_load_values();
                } else {
                    qDebug() << "Failed to open file for writing:" << jsonFilePath;
                    QMessageBox::critical(this, "Erreur Fichier", "Impossible d'écrire dans le fichier currencies.json.");
                }
            } else {
                qDebug() << "API response missing 'conversion_rates' object.";
                QMessageBox::warning(this, "Erreur API", "La réponse de l'API ne contient pas les taux de conversion.");
            }
        } else if (jsonObj.contains("result") && jsonObj["result"].toString() == "error") {
            QString errorType = jsonObj.contains("error-type") ? jsonObj["error-type"].toString() : "Unknown error";
            qDebug() << "API Error:" << errorType;
            QMessageBox::critical(this, "Erreur API", "Une erreur est survenue lors de l'appel à l'API: " + errorType);
        }
        else {
            qDebug() << "API response 'result' field missing or not 'success'.";
            QMessageBox::warning(this, "Erreur API", "La réponse de l'API n'indique pas un succès.");
        }

    } else {
        qDebug() << "Network Error:" << reply->errorString();
        QMessageBox::critical(this, "Erreur Réseau", "Une erreur réseau est survenue: " + reply->errorString());
    }

    reply->deleteLater(); // Toujours appeler deleteLater() sur QNetworkReply
}

void MainWindow::on_convert_clicked()
{
    qDebug() << "Début de la conversion...";

    // 1. Récupération des entrées
    QString amountText = ui->source_amount->toPlainText().trimmed();
    QString sourceCurrency = ui->source_currency->currentText();
    QString targetCurrency = ui->target_currency->currentText();

    qDebug() << "Montant saisi :" << amountText;
    qDebug() << "Devise source :" << sourceCurrency;
    qDebug() << "Devise cible :" << targetCurrency;

    // 2. Validation des entrées

    // Vérifier que le montant n'est pas vide
    if (amountText.isEmpty()) {
        QMessageBox::warning(this, "Erreur de saisie", "Veuillez saisir un montant à convertir.");
        return;
    }

    // Vérifier que le montant est un nombre valide
    bool conversionOk = false;
    double amount = amountText.toDouble(&conversionOk);

    if (!conversionOk) {
        QMessageBox::warning(this, "Erreur de saisie",
                             "Le montant saisi n'est pas un nombre valide.\n"
                             "Veuillez saisir un nombre (ex: 100.50).");
        return;
    }

    // Vérifier que le montant est positif
    if (amount <= 0) {
        QMessageBox::warning(this, "Erreur de saisie",
                             "Le montant doit être un nombre positif.");
        return;
    }

    // Vérifier que les devises ne sont pas vides
    if (sourceCurrency.isEmpty() || targetCurrency.isEmpty()) {
        QMessageBox::warning(this, "Erreur de sélection",
                             "Veuillez sélectionner les devises source et cible.");
        return;
    }

    // Vérifier que les devises sont présentes dans exchangeRates
    if (!exchangeRates.contains(sourceCurrency)) {
        QMessageBox::critical(this, "Erreur de taux",
                              QString("Le taux de change pour la devise source '%1' n'est pas disponible.\n"
                                      "Veuillez actualiser les taux de change.").arg(sourceCurrency));
        return;
    }

    if (!exchangeRates.contains(targetCurrency)) {
        QMessageBox::critical(this, "Erreur de taux",
                              QString("Le taux de change pour la devise cible '%1' n'est pas disponible.\n"
                                      "Veuillez actualiser les taux de change.").arg(targetCurrency));
        return;
    }

    // 3. Calcul de conversion
    double sourceRate = exchangeRates[sourceCurrency];
    double targetRate = exchangeRates[targetCurrency];

    qDebug() << "Taux source (" << sourceCurrency << ") :" << sourceRate;
    qDebug() << "Taux cible (" << targetCurrency << ") :" << targetRate;

    // Formule : Montant_Converti = (Montant_Source / Taux_Source) * Taux_Cible
    double convertedAmount = (amount / sourceRate) * targetRate;

    qDebug() << "Calcul : (" << amount << " / " << sourceRate << ") * " << targetRate << " = " << convertedAmount;

    // 4. Affichage du résultat
    QString resultText = QString::number(convertedAmount, 'f', 2);
    ui->response_text->setPlainText(resultText);

    // Message de succès dans la console
    qDebug() << "Conversion terminée avec succès :" << amount << sourceCurrency << "=" << convertedAmount << targetCurrency;

    // Optionnel : Afficher un message de confirmation plus détaillé
    QString confirmationMessage = QString("%1 %2 = %3 %4")
                                      .arg(QString::number(amount, 'f', 2))
                                      .arg(sourceCurrency)
                                      .arg(QString::number(convertedAmount, 'f', 2))
                                      .arg(targetCurrency);

    qDebug() << "Résultat formaté :" << confirmationMessage;
}


/*void MainWindow::on_refresh_clicked()
{
        qDebug() << "Refresh button clicked. Fetching latest exchange rates...";

        // 1. Définir la devise de base pour la récupération des taux
        //    Choisissez une devise de base qui vous convient, par exemple USD.
        QString baseCurrency = "USD"; // Ou "EUR", "GBP", etc.

        // 2. Construire l'URL de l'API
        //    Assurez-vous que votre 'apiKey' est correctement défini dans mainwindow.h
        QString apiUrl = QString("https://v6.exchangerate-api.com/v6/%1/latest/%2")
                             .arg(apiKey) // Utilisez la variable membre apiKey
                             .arg(baseCurrency);

        QUrl url(apiUrl);

        if (!url.isValid()) {
            qDebug() << "Invalid API URL:" << apiUrl;
            // Gérer l'erreur, par exemple afficher un message à l'utilisateur
            return;
        }

        qDebug() << "Requesting URL:" << url.toString();

        // 3. Créer une requête réseau et l'envoyer
        QNetworkRequest request(url);
        networkManager->get(request);

        // Une fois la requête envoyée, la réponse sera traitée par le slot
        // onNetworkReplyFinished que nous avons connecté précédemment.
}*/


void MainWindow::on_source_currency_activated(int index)
{

}

