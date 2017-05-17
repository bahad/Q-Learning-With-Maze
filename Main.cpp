#include <iostream>
#include <vector>
#include <array>
#include <fstream>
#include <sstream>
#include <random>
#include <algorithm>


const double G = 0.8;

double maximum(double *, int);

int maximum_indis(double *, int);

int main() {


	int baslangic, bitis, iterasyon;
	std::string dosya_adi;
	std::cout << "Acilacak input dosyası[2x2 icin 22, 3x3 icin 33]: : " << std::endl;
	std::cin >> dosya_adi;

	std::cout << "Baslangic noktası: " << std::endl;
	std::cin >> baslangic;
	std::cout << "Bitis noktasi: " << std::endl;
	std::cin >> bitis;
	std::cout << "Iterasyon sayisi: " << std::endl;
	std::cin >> iterasyon;


	std::ifstream girdi_dosyasi("input" + dosya_adi + ".txt");

	// Dosyayı 2 boyutlu (matris gibi dusun, dizi tutan dizi) iliskiler vektörüne(dinamik dizi) okuyoruz.
	/*
	* input22 icin:
	*  [
	0 => [1,2]
	1 => [0,3]
	2 => [0]
	3 => [1]
	*  ]
	*
	*  0. oda 1. ve 2. odayla komsu vs.
	*
	*/
	std::vector<std::vector<int>> iliskiler;
	for (std::string line; std::getline(girdi_dosyasi, line);) {
		std::stringstream stream(line);
		std::string ss;

		std::vector<int> tmp;
		while (getline(stream, ss, ',')) {
			tmp.push_back(std::stoi(ss));
		}
		iliskiler.push_back(tmp);
	}

	girdi_dosyasi.close();



	// R matrisi dugumsayisi x dugumsayisi boyutunda
	// Başlangıçta butun degelerini -1 yapıyoruz.
	//int R[iliskiler.size()][iliskiler.size()];

	std::vector<std::vector<int> > R(iliskiler.size(), std::vector<int>(iliskiler.size(), -1));
	//    for(int i = 0; i < iliskiler.size(); i++){
	//        for(int j = 0; j < iliskiler.size(); j++){
	//            R[i][j] = -1;
	//        }
	//    }


	// 2 Boyutlu iliskiler vektorunda gezinerek R matrisini guncelliyoruz.
	/* i = 0 =>
	j = 0 => R[0][ iliskiler[0][0] ] = 0,
	iliskiler[0][0] = 1 olduğu icin R[0][1] = 0 yaparak komsu oldugunu belirtiyoruz.
	*/
	for (int i = 0; i < iliskiler.size(); i++) {
		for (int j = 0; j < iliskiler[i].size(); j++) {
			R[i][iliskiler[i][j]] = 0;
		}
	}

	// Bitis dugumu kendisi ile komsu olmasi gerekitigi icin en son onu da guncelliyoruz
	R[bitis][bitis] = 0;




	// i'den bitis'e gecis varsa 100
	// bitis'den i'ye gecis varsa 0
	// bitis kendisi ile komsu olduğu için bitis haric
	// digerleri -1
	for (int i = 0; i < iliskiler.size(); i++) {
		if (R[i][bitis] != -1) {
			R[i][bitis] = 100;
			if (i != bitis) {
				R[bitis][i] = 0;
			}
		}
	}



	// dugumsayisi x dugumsayisi boyutunda Q matrisi.
	// double Q[iliskiler.size()][iliskiler.size()];

	std::vector<std::vector<double>> Q(iliskiler.size(), std::vector<double>(iliskiler.size(), 0));

	// baslangicta butun degeleri 0 olacak, daha sonra ogrendikce guncelleyecegiz.
	//    for(int i = 0; i < iliskiler.size(); i++){
	//        for(int j = 0; j < iliskiler.size(); j++){
	//            Q[i][j] = 0;
	//        }
	//    }

	// Random besleyici, random sayı olustururken bunlardan gelen verilere bakacak
	// srand(time(NULL)); 'deki time(NULL) kısmı bu.
	// bir tane random durum icin
	// bir tane de random eylem icin
	// ikisini de aynı random besleyiciyle beslersek aynı sayıyı uretirler
	std::mt19937 rng;   //random besleyici
	std::mt19937 rng2;   //random besleyici

	rng.seed(std::random_device()());
	rng2.seed(std::random_device()());

	// 0 - (dugumsayisi - 1) arasında
	std::uniform_int_distribution<> randDurum(0, (int)(iliskiler.size() - 1));


	int suanki_durum, eylem;
	for (int i = 0; i < iterasyon; i++) {
		suanki_durum = randDurum(rng);
		// 0 - (dugumsayisi - 1) arasında
		std::uniform_int_distribution<> randEylem(0, (int)(iliskiler.size() - 1));

		do {
			
			// suanki duruma komsu olan bir dugum bulana kadar random bir dugum indisi

			do {
				eylem = randEylem(rng2);
			} while (R[suanki_durum][eylem] == -1);

			// Qlearning formulu
			double max = *(std::max_element(Q[eylem].begin(), Q[eylem].end()));
			Q[suanki_durum][eylem] = (double)R[suanki_durum][eylem] + G * max;
			suanki_durum = eylem;

		} while (suanki_durum != bitis);
	}

	// ek kısa yolun indislerini tutmak icin dinamik bir dizi
	std::vector<int> yol;

	// baslangic dugumunu yol vektorune atıyoruz.
	int idx = baslangic;
	yol.push_back(baslangic);

	// daha sonra bitise ulasana kadar
	// baslangictan baslangictan baslayarak bitise ulasana kadar
	// Q vektorunden idx.inci satırdaki en büyük elemanın indisini bulup
	// aynı islemi tekrarlıyoruz.
	while (idx != bitis) {
		std::cout << "before idx: " << idx << "deger : " << *(std::max_element(Q[idx].begin(), Q[idx].end())) << std::endl;

		idx = (int)std::distance(Q[idx].begin(), std::max_element(Q[idx].begin(), Q[idx].end()));
		std::cout << "after idx: " << idx << std::endl;
		yol.push_back(idx);

	}


	std::ofstream cikti_dosyasi("output" + dosya_adi + ".txt");

	for (int i = 0; i < iliskiler.size(); i++) {
		for (int j = 0; j < iliskiler.size(); j++) {
			cikti_dosyasi << R[i][j] << " ";
		}
		cikti_dosyasi << "\n";
	}

	cikti_dosyasi << "\n" << "=============" << "\n\n";

	cikti_dosyasi << "Q " << iterasyon << " iterasyon sonrasi." << "\n\n";


	for (int i = 0; i < iliskiler.size(); i++) {
		for (int j = 0; j < iliskiler.size(); j++) {
			cikti_dosyasi << Q[i][j] << " ";
		}
		cikti_dosyasi << "\n";
	}

	cikti_dosyasi << "\n" << "=============" << "\n\n";

	cikti_dosyasi << "printing path\n";
	for (int i = 0; i < yol.size(); i++) {
		cikti_dosyasi << yol[i] << " ";
	}
	cikti_dosyasi << "\n";

	return 0;
}

// sıralanmamıs dizide en buyuk elemanı buluyor
// daha hızlı bulunabilirdi fakat dizinin kopyasını alıp sıralayıp
//      buyuk elemanı bulup dondurmek falan gerekiyordu
double maximum(double vektor[], int uzunluk) {
	double enbuyuk = -1;
	for (int i = 0; i < uzunluk; i++) {
		if (vektor[i] > enbuyuk) {
			enbuyuk = vektor[i];
		}
	}

	return enbuyuk;

}

int maximum_indis(double vektor[], int uzunluk) {
	double enbuyuk = -1;
	int idx = -1;
	for (int i = 0; i < uzunluk; i++) {
		if (vektor[i] > enbuyuk) {
			enbuyuk = vektor[i];
			idx = i;
		}
	}

	return idx;

}

