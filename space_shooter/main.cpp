#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <conio.h> 
#include <windows.h> 

using namespace std;

// Ekran boyuatlarý
const int GENISLIK = 30;
const int YUKSEKLIK = 20;

// Temel sýnýf: OyunNesnesi
class OyunNesnesi {
protected:
    int x, y; // Konum bilgisi
public:
    OyunNesnesi(int x, int y) : x(x), y(y) {}
    virtual void hareketEt() = 0; // Soyut hareket fonksiyonu
    int getX() const { return x; }
    int getY() const { return y; }
    void konumAta(int yeniX, int yeniY) {
        x = yeniX;
        y = yeniY;
    }
};

// Uzay Gemisi (Spaceship) Sýnýfý
class UzayGemisi : public OyunNesnesi {
public:
    UzayGemisi(int x, int y) : OyunNesnesi(x, y) {}

    void hareketEt(char yon) {
        if (yon == 'a' && x > 1) x--; // Sol
        if (yon == 'd' && x < GENISLIK - 4) x++; // Sað
    }

    void hareketEt() override {} // Boþ implementasyon
};

// Mermi (Bullet) Sýnýfý
class Mermi : public OyunNesnesi {
public:
    Mermi(int x, int y) : OyunNesnesi(x, y) {}

    void hareketEt() override {
        y--; // Mermi yukarýya doðru hareket eder
    }
};

// Düþman (Enemy) Sýnýfý
class Dusman : public OyunNesnesi {
public:
    Dusman(int x, int y) : OyunNesnesi(x, y) {}

    void hareketEt() override {
        y++; // Düþman aþaðýya doðru hareket eder
    }
};

// Oyun Sýnýfý
class Oyun {
private:
    UzayGemisi* oyuncu;
    vector<Mermi*> mermiler;
    vector<Dusman*> dusmanlar;
    bool calisiyor;
    int dusmanHizSayaci = 0;
    int yokEdilenDusmanlar = 0; // Yok edilen düþman sayýsýný tutar

    void ekranTemizle() {
        system("cls"); // Konsolu temizler
    }

    void ekranCiz() {
        char ekran[YUKSEKLIK][GENISLIK];

        // Ekraný boþluklarla doldur
        for (int i = 0; i < YUKSEKLIK; i++) {
            for (int j = 0; j < GENISLIK; j++) {
                ekran[i][j] = ' ';
            }
        }

        // Uzay gemisini yerleþtir
        ekran[oyuncu->getY()][oyuncu->getX() + 1] = '^';

        // Mermileri yerleþtir
        for (auto mermi : mermiler) {
            if (mermi->getY() >= 0)
                ekran[mermi->getY()][mermi->getX()] = '|';
        }

        // Düþmanlarý yerleþtir
        for (auto dusman : dusmanlar) {
            if (dusman->getY() < YUKSEKLIK)
                ekran[dusman->getY()][dusman->getX()] = 'X';
        }

        // Ekraný çerçeveyle çiz
        cout << " ";
        for (int i = 0; i < GENISLIK; i++) cout << "-";
        cout << endl;

        for (int i = 0; i < YUKSEKLIK; i++) {
            cout << "|"; // Sol sýnýr
            for (int j = 0; j < GENISLIK; j++) {
                cout << ekran[i][j];
            }
            cout << "|" << endl; // Sað sýnýr
        }

        cout << " ";
        for (int i = 0; i < GENISLIK; i++) cout << "-";
        cout << endl;
    }

    void dusmanUret() {
        int x = rand() % (GENISLIK - 4) + 2; // Duvarlardan 1 birim uzaklýkta konumlandýr
        dusmanlar.push_back(new Dusman(x, 0));
    }

    void carpismaKontrol() {
        for (size_t i = 0; i < mermiler.size(); i++) {
            for (size_t j = 0; j < dusmanlar.size(); j++) {
                if (mermiler[i]->getX() == dusmanlar[j]->getX() &&
                    mermiler[i]->getY() == dusmanlar[j]->getY()) {
                    // Çarpýþma durumunda, mermi ve düþmaný yok et
                    delete mermiler[i];
                    delete dusmanlar[j];
                    mermiler.erase(mermiler.begin() + i);
                    dusmanlar.erase(dusmanlar.begin() + j);
                    yokEdilenDusmanlar++; // Yok edilen düþman sayýsýný artýr
                    return;
                }
            }
        }
    }

    void geriSayim() {
        for (int i = 3; i > 0; i--) {
            ekranTemizle();
            cout << "Oyun " << i << " saniye icinde basliyor!" << endl;
            Sleep(1000);
        }
        ekranTemizle();
        cout << "Basla!" << endl;
        Sleep(1000);
        ekranTemizle();
    }

    void oyunBittiEkrani() {
        ekranTemizle();
        cout << "\n=================" << endl;
        cout << "   GAME OVER!   " << endl;
        cout << "=================" << endl;
        cout << "Yok edilen dusman sayisi: " << yokEdilenDusmanlar << endl; // Skor ekraný
    }

public:
    Oyun() {
        oyuncu = new UzayGemisi(GENISLIK / 2, YUKSEKLIK - 1); // Uzay gemisi baþlangýç konumu
        calisiyor = true;
        srand(time(0)); // Rastgele düþman konumlarý için
    }

    ~Oyun() {
        delete oyuncu;
        for (auto mermi : mermiler) delete mermi;
        for (auto dusman : dusmanlar) delete dusman;
    }

    void calistir() {
        geriSayim();

        while (calisiyor) {
            // Klavye giriþi
            if (_kbhit()) {
                char tus = _getch();
                if (tus == 'a' || tus == 'd') oyuncu->hareketEt(tus);
                if (tus == ' ') mermiler.push_back(new Mermi(oyuncu->getX() + 1, oyuncu->getY() - 1));
                if (tus == 'q') calisiyor = false; // Oyunu bitir
            }

            // Düþman üret
            if (rand() % 15 == 0) dusmanUret();

            // Düþmanlarý daha yavaþ hareket ettir
            dusmanHizSayaci++;
            if (dusmanHizSayaci > 5) {
                for (auto dusman : dusmanlar) dusman->hareketEt();
                dusmanHizSayaci = 0;
            }

            // Mermileri güncelle
            for (auto mermi : mermiler) mermi->hareketEt();

            // Çarpýþma kontrolü
            carpismaKontrol();

            // Ekranýn altýna ulaþan düþman kontrolü
            for (auto dusman : dusmanlar) {
                if (dusman->getY() >= YUKSEKLIK) {
                    calisiyor = false;
                    oyunBittiEkrani();
                }
            }

            // Ekraný temizle ve yeniden çiz
            ekranTemizle();
            ekranCiz();

            Sleep(100); // Oyun döngüsünü yavaþlat
        }

        // Oyun bittiðinde, sonuç ekraný göster
        oyunBittiEkrani();
    }
};

int main() {
    Oyun oyun;
    oyun.calistir();
    return 0;
}
