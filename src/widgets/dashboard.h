#ifndef DASHBOARD_H
#define DASHBOARD_H

#include <QWidget>

class QGridLayout;
class QLabel;
class QPushButton;
class QLineEdit;

class AppSettings;

class Dashboard : public QWidget
{
    Q_OBJECT
public:
    explicit Dashboard(AppSettings &settings, QWidget *parent = nullptr);

    void initWidgets();

signals:

private slots:
    void selectGermanyFolder();
    void selectJhuFolder();
    void germanyFolderChanged(const QString &text);
    void jhuFolderChanged(const QString &text);

private:
    AppSettings &appSettings;

    QGridLayout *gLayout;

    QLabel *germanyDataLabel;
    QLabel *germanyDescriptionLabel;
    QLineEdit *germanyDataLineEdit;
    QPushButton *germanyDataSearchPathButton;

    QLabel *jhuDataLabel;
    QLabel *jhuDescriptionLabel;
    QLineEdit *jhuDataLineEdit;
    QPushButton *jhuDataSearchPathButton;

    QLabel *infoLabel;

    QPushButton *loadDataButton;
};

#endif // DASHBOARD_H
