#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QNetworkAccessManager> // Pour les requêtes réseau
#include <QNetworkReply>       // Pour gérer les réponses réseau
#include <QUrl>                // Pour construire l'URL de l'API
#include <QJsonDocument>       // Pour lire/écrire des documents JSON
#include <QJsonObject>         // Pour manipuler des objets JSON
#include <QFile>               // Pour lire/écrire des fichiers
#include <QDebug>              // Pour le débogage (optionnel mais utile)
#include <QMessageBox> // Pour afficher des messages à l'utilisateur
#include <QJsonArray> // Peut être utile si l'API retourne des arrays


QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_convert_clicked();

    void on_refresh_clicked();

    void onNetworkReplyFinished(QNetworkReply *reply);

private:
    Ui::MainWindow *ui;
    QNetworkAccessManager *networkManager; // Déclarez ceci
    QString apiKey = "525b9c3520a6fdfc47e7d454"; // Déclarez votre clé d'API
    QString jsonFilePath = "currencies.json"; // Chemin vers votre fichier JSON
};
#endif // MAINWINDOW_H
