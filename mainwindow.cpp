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
}

MainWindow::~MainWindow()
{
    delete ui;
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

                    // TODO: Mettre à jour les QComboBoxes ici après la sauvegarde
                    // Vous pouvez appeler une fonction comme populateComboBoxes();
                    // que nous développerons par la suite.

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

