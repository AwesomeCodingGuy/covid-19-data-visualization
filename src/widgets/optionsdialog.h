#ifndef OPTIONSDIALOG_H
#define OPTIONSDIALOG_H

#include <QDialog>

class QRadioButton;
class QPushButton;
class QButtonGroup;
class QLabel;

class AppSettings;

class OptionsDialog : public QDialog
{
    Q_OBJECT

    enum OptionsLanguageId {
        English,
        German
    };

public:
    explicit OptionsDialog(AppSettings *settings, QWidget *parent = nullptr);

    void initUi();

protected:
    virtual void changeEvent(QEvent *event);

private:
    void retranslateUi();

public slots:
    void radioChanged(int id, bool checked);

signals:

private:
    AppSettings *settings;

    QPushButton *closeButton;
    QLabel *languageLabel;
    QLabel *infoLabel;
    QButtonGroup *radioButtonGroup;
    QRadioButton *englishRadio;
    QRadioButton *germanRadio;
};

#endif // OPTIONSDIALOG_H
