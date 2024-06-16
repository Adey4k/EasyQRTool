#include <iostream>
#include <string>
#include <bitset>
#include <vector>
#include <qrencode.h>
#include <fstream>
#include <iomanip>
#include <windows.h>

using namespace std;

int getConsoleWidth() {
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    return csbi.srWindow.Right - csbi.srWindow.Left + 1;
}

void centerText(const std::string& text) {
    int width = getConsoleWidth();
    int leftPadding = (width - text.length()) / 2;
    std::cout << std::setw(leftPadding + text.length()) << text << std::endl;
}

// Заповнення бінарного рядка нулями та спеціальними байтами для корекції
string fullfill(string Binary, unsigned int maxsize) {
    Binary.append("0000"); // Додаємо кінцеві біти
    // Заповнюємо нулями до найближчого кратного 8
    if (Binary.size() % 8 != 0) {
        Binary.append(string((8 - Binary.size() % 8), '0'));
    }
    string paddingBytes[2] = { "11101100", "00010001" };
    size_t paddingIndex = 0;
    while (Binary.size() < maxsize) {
        Binary.append(paddingBytes[paddingIndex]);
        paddingIndex = (paddingIndex + 1) % 2;
    }
    return Binary;
}

// Функція пошуку індексу елемента в векторі
int gf_indexOf(const vector<int>& gf, int value) {
    for (size_t i = 0; i < gf.size(); ++i) {
        if (gf[i] == value) {
            return i;
        }
    }
    return -1;
}

// Множення в полях Галуа
int gf_multiply(int a, int b, const vector<int>& gf) {
    if (a == 0 || b == 0) return 0;
    return gf[(gf_indexOf(gf, a) + gf_indexOf(gf, b)) % 255];
}

// Додавання байтів корекції помилок
vector<int> addCorrectionBytes(const vector<int>& dataBlock, const vector<int>& generator, const vector<int>& gf) {
    vector<int> block = dataBlock;
    block.insert(block.end(), generator.size() - 1, 0);
    for (size_t i = 0; i < dataBlock.size(); ++i) {
        if (block[i] == 0) continue;
        int coef = block[i];
        for (size_t j = 0; j < generator.size(); ++j) {
            block[i + j] ^= gf_multiply(coef, generator[j], gf);
        }
    }
    return vector<int>(block.begin() + dataBlock.size(), block.end());
}

// Створення QR-коду та збереження його у файл
bool createQRCode(const string& data, const string& filename, QRecLevel errorCorrectionLevel) {
    QRcode* qr = QRcode_encodeString(data.c_str(), 0, errorCorrectionLevel, QR_MODE_8, 1);
    if (qr == nullptr) {
        cerr << "Failed to create QR code" << endl;
        return false;
    }
    ofstream outFile(filename, ios::binary);
    if (!outFile.is_open()) {
        cerr << "Failed to open file for writing" << endl;
        QRcode_free(qr);
        return false;
    }
    int moduleSize = 10;
    int qrWidth = qr->width * moduleSize;
    outFile << "P5\n" << qrWidth << " " << qrWidth << "\n255\n";
    for (int y = 0; y < qr->width; ++y) {
        for (int ys = 0; ys < moduleSize; ++ys) {
            for (int x = 0; x < qr->width; ++x) {
                unsigned char color = qr->data[y * qr->width + x] & 1 ? 0 : 255;
                for (int xs = 0; xs < moduleSize; ++xs) {
                    outFile << color;
                }
            }
        }
    }
    outFile.close();
    QRcode_free(qr);
    return true;
}

int main() {
    string Str;
    string Binary;

    centerText("Enter info for QR - code: ");
    centerText("(You can't use symbols that are not English letters, numbers, or some ASCII symbols (like /, :, ., etc))");

    bool charactersValid = false;

    while (!charactersValid) {
        getline(cin, Str);
        if (Str.empty()) {
            centerText("No input detected. Please enter some information.");
            continue;
        }

        charactersValid = true;
        Binary.clear();

        for (char c : Str) {
            if (bitset<8>(c) == bitset<8>(0b00111111)) {
                centerText("Invalid characters detected. Try again.");
                charactersValid = false;
                break;
            }
            Binary += bitset<8>(c).to_string();
        }
    }
    centerText("Select error correction level: Print L or M or Q or H");
    centerText("Where L - 7%, M - 15%, Q - 25%, H - 30%: ");
    char level;
    cin >> level;
    QRecLevel errorCorrectionLevel;
    int ErrLvl;
    switch (level) {
    case 'L':
        errorCorrectionLevel = QR_ECLEVEL_L;
        ErrLvl = 0;
        break;
    case 'M':
        errorCorrectionLevel = QR_ECLEVEL_M;
        ErrLvl = 1;
        break;
    case 'Q':
        errorCorrectionLevel = QR_ECLEVEL_Q;
        ErrLvl = 2;
        break;
    case 'H':
        errorCorrectionLevel = QR_ECLEVEL_H;
        ErrLvl = 3;
        break;
    default:
        cerr << "Invalid level. Defaulting to M." << endl;
        errorCorrectionLevel = QR_ECLEVEL_M;
        ErrLvl = 1;
    }

    // Визначаємо максимальний розмір для бінарних даних
    vector<vector<int>> maxsize = {
        {152, 272, 440, 640, 864, 1088}, // L
        {128, 224, 352, 512, 688, 864},  // M
        {104, 176, 272, 384, 496, 608},  // Q
        {72, 128, 208, 288, 368, 480}    // H
    };

    int version = 1; // Початкова версія QR-коду

    // Визначаємо необхідний розмір для повноти даних
    if (Binary.size() < maxsize[ErrLvl][0]) {
        Binary = fullfill(Binary, maxsize[ErrLvl][0]);
    }
    else {
        bool versionFound = false;
        for (int i = 1; i < 7; i++) {
            if (Binary.size() < maxsize[ErrLvl][i]) {
                Binary = fullfill(Binary, maxsize[ErrLvl][i]);
                version = i + 1;
                versionFound = true;
                break;
            }
        }
        if (!versionFound) {
            cerr << "QR-code must be less than " << maxsize[ErrLvl].back() << " binary symbols." << endl;
            cerr << "Try using lower Error correction level or reduce input length." << endl;
            return 1;
        }
    }

    // Перетворюємо бінарні дані в байти
    vector<int> Kblock;
    for (size_t i = 0; i < Binary.size(); i += 8) {
        string byteStr = Binary.substr(i, 8);
        int byteValue = stoi(byteStr, nullptr, 2);
        Kblock.push_back(byteValue);
    }

    // Генеруємо поле Галуа
    int prim_poly = 0x11d;
    vector<int> gf(256);
    gf[0] = 1;
    for (int i = 1; i < 256; ++i) {
        int value = gf[i - 1] << 1;
        if (value & 0x100) {
            value ^= prim_poly;
        }
        gf[i] = value;
    }

    // Визначаємо генератор для версії QR-коду
    vector<int> generator;
    if (ErrLvl == 0) {
        switch (version) {
        case 1:
            generator = { 1, 87, 229, 146, 149, 238, 102, 21 };
            break;
        case 2:
            generator = { 1, 251, 67, 46, 61, 118, 70, 64, 94, 32, 45 };
            break;
        case 3:
            generator = { 1, 8, 183, 61, 91, 202, 37, 51, 58, 58, 237, 140, 124, 5, 99, 105 };
            break;
        case 4:
            generator = { 1, 17, 60, 79, 50, 61, 163, 26, 187, 202, 180, 221, 225, 83, 239, 156, 164, 212, 212, 188, 190 };
            break;
        case 5:
            generator = { 1, 173, 125, 158, 2, 103, 182, 118, 17, 145, 201, 111, 28, 165, 53, 161, 21, 245, 142, 13, 102, 48, 227, 153, 145, 218, 70 };
            break;
        case 6:
            generator = { 1, 87, 229, 146, 149, 238, 102, 21 }; // Виправлений генераторний многочлен
            break;
        default:
            cerr << "Unsupported version: " << version << endl;
            return 1;
        }
    }
    else if (ErrLvl == 1) {
        switch (version) {
        case 1:
            generator = { 1, 251, 67, 46, 61, 118, 70, 64, 94, 32, 45 };
            break;
        case 2:
            generator = { 1, 120, 104, 107, 109, 102, 161, 76, 3, 91, 191, 147, 169, 182, 194, 225, 120 };
            break;
        case 3:
            generator = { 1, 173, 125, 158, 2, 103, 182, 118, 17, 145, 201, 111, 28, 165, 53, 161, 21, 245, 142, 13, 102, 48, 227, 153, 145, 218, 70 };
            break;
        case 4:
            generator = { 1, 215, 234, 158, 94, 184, 97, 118, 170, 79, 187, 152, 148, 252, 179, 5, 98, 96, 153 };
            break;
        case 5:
            generator = { 1, 229, 121, 135, 48, 211, 117, 251, 126, 159, 180, 169, 152, 192, 226, 228, 218, 111, 0, 117, 232, 87, 96, 227, 21 };
            break;
        case 6:
            generator = { 1, 120, 104, 107, 109, 102, 161, 76, 3, 91, 191, 147, 169, 182, 194, 225, 120 };
            break;
        default:
            cerr << "Unsupported version: " << version << endl;
            return 1;
        }
    }
    else if (ErrLvl == 2) {
        switch (version) {
        case 1:
            generator = { 1, 74, 152, 176, 100, 86, 100, 106, 104, 130, 218, 206, 140, 78 };
            break;
        case 2:
            generator = { 1, 210, 171, 247, 242, 93, 230, 14, 109, 221, 53, 200, 74, 8, 172, 98, 80, 219, 134, 160, 105, 165, 231 };
            break;
        case 3:
            generator = { 1, 215, 234, 158, 94, 184, 97, 118, 170, 79, 187, 152, 148, 252, 179, 5, 98, 96, 153 };
            break;
        case 4:
            generator = { 1, 173, 125, 158, 2, 103, 182, 118, 17, 145, 201, 111, 28, 165, 53, 161, 21, 245, 142, 13, 102, 48, 227, 153, 145, 218, 70 };
            break;
        case 5:
            generator = { 1, 215, 234, 158, 94, 184, 97, 118, 170, 79, 187, 152, 148, 252, 179, 5, 98, 96, 153 };
            break;
        case 6:
            generator = { 1, 229, 121, 135, 48, 211, 117, 251, 126, 159, 180, 169, 152, 192, 226, 228, 218, 111, 0, 117, 232, 87, 96, 227, 21 };
            break;
        default:
            cerr << "Unsupported version: " << version << endl;
            return 1;
        }
    }
    else if (ErrLvl == 3) {
        switch (version) {
        case 1:
            generator = { 1, 43, 139, 206, 78, 43, 239, 123, 206, 214, 147, 24, 99, 150, 39, 243, 163, 136 };
            break;
        case 2:
            generator = { 1, 168, 223, 200, 104, 224, 234, 108, 180, 110, 190, 195, 147, 205, 27, 232, 201, 21, 43, 245, 87, 42, 195, 212, 119, 242, 37, 9, 123 };
            break;
        case 3:
            generator = { 1, 210, 171, 247, 242, 93, 230, 14, 109, 221, 53, 200, 74, 8, 172, 98, 80, 219, 134, 160, 105, 165, 231 };
            break;
        case 4:
            generator = { 1, 120, 104, 107, 109, 102, 161, 76, 3, 91, 191, 147, 169, 182, 194, 225, 120 };
            break;
        case 5:
            generator = { 1, 210, 171, 247, 242, 93, 230, 14, 109, 221, 53, 200, 74, 8, 172, 98, 80, 219, 134, 160, 105, 165, 231 };
            break;
        case 6:
            generator = { 1, 168, 223, 200, 104, 224, 234, 108, 180, 110, 190, 195, 147, 205, 27, 232, 201, 21, 43, 245, 87, 42, 195, 212, 119, 242, 37, 9, 123 };
            break;
        default:
            cerr << "Unsupported version: " << version << endl;
            return 1;
        }
    }

    // Додаємо байти корекції помилок
    vector<int> correctionBytes = addCorrectionBytes(Kblock, generator, gf);

    // Об'єднуємо байти даних та корекції
    vector<int> FinalBytes;
    FinalBytes.reserve(Kblock.size() + correctionBytes.size());
    FinalBytes.insert(FinalBytes.end(), Kblock.begin(), Kblock.end());
    FinalBytes.insert(FinalBytes.end(), correctionBytes.begin(), correctionBytes.end());

    // Кодуємо остаточні дані в рядок
    string encodedData = "";
    for (int byte : FinalBytes) {
        encodedData += static_cast<char>(byte);
    }

    // Створюємо QR-код та зберігаємо у файл
    string filename = "qrcode.pgm";
    if (createQRCode(encodedData, filename, errorCorrectionLevel)) {
        string ver = to_string(version);
        centerText("QR Code saved to " + filename);
        centerText("QR Code version: " + ver);
    }
    else {
        centerText("Failed to create or save QR Code. Please try again.");
    }

    system("PAUSE");
    return 0;
}
