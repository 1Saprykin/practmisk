#include <iostream>
#include <string>
#include <fstream>
#include <ctime>

#define _CRT_SECURE_NO_WARNINGS
#pragma warning(disable : 4996)

using namespace std;

struct USER {
    string Login;
    string Password;
    int Rights = 1;
    bool IsBlocked = false;
    int FailedAttempts = 0;
};
USER U;

void Book(string text);
bool Authentication();
void ReadFiles();
void WriteFiles();
void CreateFiles();
void DeleteFiles();
void CopyFiles();
void MoveFiles();
void UserActions();
void CreateUser();
void DeleteUser();
void Rights();
void BlockUser();
void UnblockUser();
void AdminAction();

int main() {
    setlocale(LC_ALL, "");
    char change = '0';
    while (change != '2') {
        cout << "Меню: 1.Аутентификация 2.Выход" << endl;
        cin >> change;
        if (change == '1') {
            if (Authentication()) {
                if (U.Login == "ADMIN") {
                    AdminAction();
                } else {
                    UserActions();
                }
            }
        } else if (change != '2') {
            cout << "Ошибка: выбрано неверное действие!" << endl;
        }
    }
    return 0;
}

void Book(string text) {
    time_t now = time(0);
    char* dt = ctime(&now);
    ofstream Book("/home/dmirtil/Lab/Praktika/Book.txt", ios_base::app);
    Book << dt << text << endl << endl;
    Book.close();
}

bool Authentication() {
    cout << "Аутентификация" << endl;
    const int max_attempts = 3;

    cout << "Введите логин пользователя: ";
    cin >> U.Login;
    cout << "Введите пароль: ";
    cin >> U.Password;

    if (U.Login == "ADMIN" && U.Password == "admin123") {
        return true;
    }

    ifstream Data("/home/dmirtil/Lab/Praktika/" + U.Login + ".txt");
    if (Data.is_open()) {
        string Correct_Password;
        Data >> Correct_Password >> U.Rights >> U.IsBlocked >> U.FailedAttempts;
        Data.close();

        if (U.IsBlocked) {
            cout << "Ошибка: Учетная запись заблокирована!" << endl;
            return false;
        }

        if (Correct_Password == U.Password) {
            U.FailedAttempts = 0;
            ofstream DataOut("/home/dmirtil/Lab/Praktika/" + U.Login + ".txt");
            DataOut << Correct_Password << endl << U.Rights << endl << U.IsBlocked << endl << U.FailedAttempts;
            DataOut.close();
            return true;
        } else {
            cout << "Ошибка: Введен неверный пароль!" << endl;
            U.FailedAttempts++;
            if (U.FailedAttempts >= max_attempts) {
                U.IsBlocked = true;
                cout << "Учетная запись заблокирована из-за превышения числа неудачных попыток входа!" << endl;
            }
            ofstream DataOut("/home/dmirtil/Lab/Praktika/" + U.Login + ".txt");
            DataOut << Correct_Password << endl << U.Rights << endl << U.IsBlocked << endl << U.FailedAttempts;
            DataOut.close();
            return false;
        }
    } else {
        cout << "Ошибка: Введен неверный логин!" << endl;
        return false;
    }
}

void ReadFiles() {
    if (U.Rights & (1 << 0)) {
        string FName, Line;
        cout << "Введите название файла: ";
        cin >> FName;
        ifstream RFile(FName);
        if (RFile.is_open()) {
            cout << "---------------------------------------------" << endl;
            while (getline(RFile, Line)) {
                cout << Line << endl;
            }
            cout << "---------------------------------------------" << endl;
            RFile.close();
            Book("Файл " + FName + " успешно прочитан!");
        } else {
            cout << "Ошибка: Файл <" << FName << "> не найден!" << endl;
        }
    } else {
        cout << "Ошибка: У пользователя <" << U.Login << "> отсутствуют права на чтение файлов!" << endl;
    }
}
void WriteFiles() {
    if (U.Rights & (1 << 1)) {
        string FName, Line;
        cout << "Введите название файла: ";
        cin >> FName;
        ofstream WFile(FName, ios_base::app);
        cout << "Введите текст для записи: ";
        cin.ignore(); // чтобы игнорировать оставшийся '\n' после cin
        getline(cin, Line);
        WFile << Line;
        WFile.close();
        Book("Была произведена запись в файл " + FName);
    } else {
        cout << "Ошибка: У пользователя <" << U.Login << "> отсутствуют права на запись!" << endl;
    }
}

void CreateFiles() {
    if (U.Rights & (1 << 2)) {
        string FName;
        cout << "Введите название файла: ";
        cin >> FName;
        ofstream NewFile(FName);
        NewFile.close();
        cout << "Файл " << FName << " успешно создан!" << endl;
        Book("Файл " + FName + " успешно создан!");
    } else {
        cout << "Ошибка: У пользователя <" << U.Login << "> отсутствуют права на создание файлов" << endl;
    }
}

void DeleteFiles() {
    if (U.Rights & (1 << 3)) {
        cout << "Введите название файла: ";
        string FName;
        cin >> FName;
        if (remove(FName.c_str()) == 0) {
            cout << "Файл " << FName << " успешно удален!";
            Book("Файл " + FName + " успешно удален!");
        } else {
            cout << "Ошибка: Не удалось удалить файл " << FName << "!" << endl;
        }
    } else {
        cout << "Ошибка: У пользователя <" << U.Login << "> отсутствуют права на удаление файла" << endl;
    }
}

void CopyFiles() {
    if (U.Rights & (1 << 4)) {
        string srcFile, destFile;
        cout << "Введите название исходного файла: ";
        cin >> srcFile;
        cout << "Введите название файла назначения: ";
        cin >> destFile;
        ifstream src(srcFile, ios::binary);
        ofstream dest(destFile, ios::binary);
        dest << src.rdbuf();
        src.close();
        dest.close();
        cout << "Файл " << srcFile << " успешно скопирован в " << destFile << "!" << endl;
        Book("Файл " + srcFile + " успешно скопирован в " + destFile);
    } else {
        cout << "Ошибка: У пользователя <" << U.Login << "> отсутствуют права на копирование файлов" << endl;
    }
}

void MoveFiles() {
    if (U.Rights & (1 << 5)) {
        string srcFile, destFile;
        cout << "Введите название исходного файла: ";
        cin >> srcFile;
        cout << "Введите название файла назначения: ";
        cin >> destFile;
        if (rename(srcFile.c_str(), destFile.c_str()) == 0) {
            cout << "Файл " << srcFile << " успешно перемещен в " << destFile << "!" << endl;
            Book("Файл " + srcFile + " успешно перемещен в " + destFile);
        } else {
            cout << "Ошибка: Не удалось переместить файл " << srcFile << " в " << destFile << "!" << endl;
        }
    } else {
        cout << "Ошибка: У пользователя <" << U.Login << "> отсутствуют права на перемещение файлов" << endl;
    }
}

void UserActions() {
    while (true) {
        cout << "Выберите действие (нажмите на нужную цифру, соответствующую действию): " << endl;
        cout << "\t\t1.Чтение файлов\n\t\t2.Запись в файл\n\t\t3.Создание файла\n\t\t4.Удаление файла\n\t\t5.Копирование файла\n\t\t6.Перемещение файла\n\t\t7.Выход\n";
        char choice;
        cin >> choice;
        if (choice == '1')
            ReadFiles();
        else if (choice == '2')
            WriteFiles();
        else if (choice == '3')
            CreateFiles();
        else if (choice == '4')
            DeleteFiles();
        else if (choice == '5')
            CopyFiles();
        else if (choice == '6')
            MoveFiles();
        else if (choice == '7')
            break;
        else
            cout << "Ошибка: Выбрано несуществующее действие!" << endl;
    }
}
void CreateUser() {
    string Login, Password;
    int Rights = 0;
    cout << "Введите логин нового пользователя: ";
    cin >> Login;
    cout << "Введите пароль нового пользователя: ";
    cin >> Password;
    ofstream Data("/home/dmirtil/Lab/Praktika/" + Login + ".txt");
    Data << Password << endl;
    Data << Rights << endl;
    Data << "0" << endl; // IsBlocked
    Data << "0" << endl; // FailedAttempts
    Data.close();
    cout << "Пользователь <" << Login << "> успешно создан!" << endl;
    Book("Создан новый пользователь: " + Login);
}

void DeleteUser() {
    string Login;
    cout << "Введите логин пользователя: ";
    cin >> Login;
    string path = "/home/dmirtil/Lab/Praktika/" + Login + ".txt";
    ifstream Data(path);
    if (Data.is_open()) {
        Data.close();
        remove(path.c_str());
        cout << "Пользователь <" << Login << "> успешно удален!" << endl;
        Book("Удален пользователь: " + Login);
    } else {
        cout << "Ошибка: Пользователя <" << Login << "> не существует!" << endl;
    }
}

void Rights() {
    string User, Password;
    int Rights_ = 0;
    bool IsBlocked;
    int FailedAttempts;
    char a = 0;
    cout << "Введите логин пользователя: ";
    cin >> User;
    string path = "/home/dmirtil/Lab/Praktika/" + User + ".txt";
    ifstream FileUser(path);
    if (FileUser.is_open()) {
        FileUser >> Password >> Rights_ >> IsBlocked >> FailedAttempts;
        FileUser.close();
metka1:
        cout << "Введите право на чтение (1 - есть, 0 - нет): " << endl;
        cin >> a;
        if (a == '1')
            Rights_ |= (1 << 0);
        else if (a == '0')
            Rights_ &= ~(1 << 0);
        else {
            cout << "Ошибка: Выбрано неверное действие!" << endl;
            goto metka1;
        }
metka2:
        cout << "Введите право на запись (1 - есть, 0 - нет): " << endl;
        cin >> a;
        if (a == '1')
            Rights_ |= (1 << 1);
        else if (a == '0')
            Rights_ &= ~(1 << 1);
        else {
            cout << "Ошибка: Выбрано неверное действие!" << endl;
            goto metka2;
        }
metka3:
        cout << "Введите право на создание (1 - есть, 0 - нет): " << endl;
        cin >> a;
        if (a == '1')
            Rights_ |= (1 << 2);
        else if (a == '0')
            Rights_ &= ~(1 << 2);
        else {
            cout << "Ошибка: Выбрано неверное действие!" << endl;
            goto metka3;
        }
metka4:
        cout << "Введите право на удаление (1 - есть, 0 - нет): " << endl;
        cin >> a;
        if (a == '1')
            Rights_ |= (1 << 3);
        else if (a == '0')
            Rights_ &= ~(1 << 3);
        else {
            cout << "Ошибка: Выбрано неверное действие!" << endl;
            goto metka4;
        }
metka5:
        cout << "Введите право на копирование (1 - есть, 0 - нет): " << endl;
        cin >> a;
        if (a == '1')
            Rights_ |= (1 << 4);
        else if (a == '0')
            Rights_ &= ~(1 << 4);
        else {
            cout << "Ошибка: Выбрано неверное действие!" << endl;
            goto metka5;
        }
metka6:
        cout << "Введите право на перемещение (1 - есть, 0 - нет): " << endl;
        cin >> a;
        if (a == '1')
            Rights_ |= (1 << 5);
        else if (a == '0')
            Rights_ &= ~(1 << 5);
        else {
            cout << "Ошибка: Выбрано неверное действие!" << endl;
            goto metka6;
        }
        ofstream NewFileUser(path);
        NewFileUser << Password << endl;
        NewFileUser << Rights_ << endl;
        NewFileUser << IsBlocked << endl;
        NewFileUser << FailedAttempts;
        NewFileUser.close();
        cout << "Права пользователю <" << User << "> успешно переназначены" << endl;
        Book("Назначены новые права пользователю: " + User);
    } else {
        cout << "Ошибка: Пользователя <" << User << "> не существует!" << endl;
    }
}
void BlockUser() {
    string Login;
    cout << "Введите логин пользователя для блокировки: ";
    cin >> Login;
    string path = "/home/dmirtil/Lab/Praktika/" + Login + ".txt";
    ifstream Data(path);
    if (Data.is_open()) {
        string Password;
        int Rights;
        bool IsBlocked;
        int FailedAttempts;
        Data >> Password >> Rights >> IsBlocked >> FailedAttempts;
        Data.close();
        if (IsBlocked) {
            cout << "Ошибка: Пользователь уже заблокирован!" << endl;
        } else {
            IsBlocked = true;
            ofstream DataOut(path);
            DataOut << Password << endl;
            DataOut << Rights << endl;
            DataOut << IsBlocked << endl;
            DataOut << FailedAttempts;
            DataOut.close();
            cout << "Пользователь <" << Login << "> успешно заблокирован!" << endl;
            Book("Пользователь " + Login + " заблокирован.");
        }
    } else {
        cout << "Ошибка: Пользователя <" << Login << "> не существует!" << endl;
    }
}

void UnblockUser() {
    string Login;
    cout << "Введите логин пользователя для разблокировки: ";
    cin >> Login;
    string path = "/home/dmirtil/Lab/Praktika/" + Login + ".txt";
    ifstream Data(path);
    if (Data.is_open()) {
        string Password;
        int Rights;
        bool IsBlocked;
        int FailedAttempts;
        Data >> Password >> Rights >> IsBlocked >> FailedAttempts;
        Data.close();
        if (!IsBlocked) {
            cout << "Ошибка: Пользователь не заблокирован!" << endl;
        } else {
            IsBlocked = false;
            FailedAttempts = 0;
            ofstream DataOut(path);
            DataOut << Password << endl;
            DataOut << Rights << endl;
            DataOut << IsBlocked << endl;
            DataOut << FailedAttempts;
            DataOut.close();
            cout << "Пользователь <" << Login << "> успешно разблокирован!" << endl;
            Book("Пользователь " + Login + " разблокирован.");
        }
    } else {
        cout << "Ошибка: Пользователя <" << Login << "> не существует!" << endl;
    }
}

void AdminAction() {
    while (true) {
        cout << "Выберите действие (нажмите на нужную цифру, соответствующую действию): " << endl;
        cout << "\t\t1.Создание пользователя\n\t\t2.Удаление пользователя\n\t\t3.Назначение полномочий\n\t\t4.Блокирование пользователя\n\t\t5.Разблокирование пользователя\n\t\t6.Выход\n";
        char choice;
        cin >> choice;
        if (choice == '1')
            CreateUser();
        else if (choice == '2')
            DeleteUser();
        else if (choice == '3')
            Rights();
        else if (choice == '4')
            BlockUser();
        else if (choice == '5')
            UnblockUser();
        else if (choice == '6')
            break;
        else
            cout << "Ошибка: Выбрано несуществующее действие!" << endl;
    }
}
