#include<iostream>
#include<fstream>
#include<string>


class Hashtable {

private:
	int sum;					//zmienna ktora liczy ile razy dodalismy i odjelismy element, tak zeby ilosc elementow nie przekroczyla wielkosci tab
	std::fstream readFile;		//plik do odczytu
	std::fstream saveFile;		//plik do zapisu
	std::string readFileName;	//nazwa pliku do odczytu
	std::string saveFileName;	//nazwa pliku do zapisu
	int repeat;					//ile razy wykonujemy program (1 wartosc w pliku)
	std::string information;	//przechowuje wartosci odczytane z pliku typu delete, add itp...
	int arraySize;				//wielkosc tablicy
	int key;					//przechowuje kolejne wartosci odczytane z pliku, te ktore okresla klucz (np. 132, 13, 99, 9)
	std::string text;			//przechowuje dalsze wartosci odczytane z pliku, te ktore wystepuja po kluczu (w pliku zrodlowym np. litera D)
	int position;				//przechowuje miejsce zapisu elementu (klucz % wielkosc tablicy) 
	std::string* array;			//tablica na ktorej pracujemy		
	int* arrayTemp;				//tablica dodatkowa w ktorej zapisywane sa klucze, wykorzystywana do dzialania algorytmu 
	int keyToString;			//zmienna ktora uzyjemy do zmiany wartosci key z int na string


public:
	Hashtable() {
		sum = 0;
		std::ifstream readFile;
		readFileName = "dane";
		saveFileName = "";
		repeat = 0;
		information = "";
		arraySize = 0;
		key = 0;
		text = "";
		position = 0;
		keyToString =  0;
	}

	void createTest() {

		std::cout << "Stworz plik do zapisu (podaj nazwe): ";
		std::cin >> saveFileName;

		readFile.open(readFileName, std::ios::in);
		saveFile.open(saveFileName, std::ios::out | std::ios::app);

		if (readFile.good() == true) {
				
			readFile >> repeat;
			
			for(int i=0; i<repeat; i++){

				do {

					readFile >> information;

					if (information == "size") {
						setSize();
					}
					else if (information == "add") {
						//ZABEZPIECZENIE WIECEJ ELEMENTOW NIZ PODANA WIELKOSC TABLICY
						if (sum < arraySize) {
							addElement();
							sum += 1;
						}
						else {
							saveFile << "To many elements, size of array set in file 'dane' is not enough, change data in file 'dane'..." <<std::endl;
						}
					}
					else if (information == "delete") {
						deleteElement();
						sum -= 1;
					}
					else if (information == "print") {
						printHashTable();
					}
					else if (information == "stop") {
						stop();
					}
				
				} while (information != "stop");
			}
		}
	}

	void setSize() {

		readFile >> arraySize;
		array = new std::string[arraySize];
		arrayTemp = new int[arraySize];

	}

	void addElement() {
	
		//tworzenie zmiennej utrzymujacej pozycje elementu
		readFile >> key;
		position = key % arraySize;
		
		//tworzenie calosci komorki
		keyToString = key;
		std::string str = std::to_string(keyToString);
		readFile >> text;
		std::string element;
		element = str + " " + text;
		
		//przypisanie elementu do odpowiedniej pozycji 
		int i = 0;
		bool quit = false;
		i = 0;
		do {
			if (array[position+i] == element) {
				quit = true;
			}
			else if (array[position + i] == "") {
				array[position + i] = element;
			}
			else if ((position+i)==arraySize) {
				i = 0;
				position = 0;
			}
			else
			{
				i++;
			}
		} while (array[position + i] != element || quit==false);
		

			arrayTemp[position + i] = key;
		
	}

	void deleteElement() {

		readFile >> key;
		position = key % arraySize;
		
		bool done = false;
		int deletedElementPosition = 0;
		int i = 0;
		
		while (array[position + i] != "") {
			if (key == arrayTemp[position + i]) {
				array[position+i] = "";
				//zmiana dla klucza
				arrayTemp[position + i] = NULL;
				deletedElementPosition = position + i;
			}
			else if ((i + position == arraySize - 1) && done == false) {
				i = 0;
				position = 0;
				done = true;
			}
			else {
				i++;
			}
		}

		//metoda ktora sprawdzi kolejne elementy o tym samym kluczu
		checkNext(deletedElementPosition);
		
	}
	

	void printHashTable() {

		for (int i = 0; i < arraySize; i++) {
			if (array[i] != "") {
				saveFile << i << " " << array[i] << std::endl;
			}
		}
		saveFile << std::endl;
	}

	void stop() {
		delete[] array;
	}

	void checkNext(int j){
		int i = 0;
		bool quit = false;
		do {
			if (j == arraySize - 1 && (arrayTemp[0] % arraySize == key % arraySize)) {
				array[j] = array[0];
				array[0] = "";
				//zmiana dla klucza
				arrayTemp[j] = arrayTemp[0];
				arrayTemp[0] = NULL;
				j = 0;
			}
			else if (arrayTemp[j + i + 1] % arraySize == key % arraySize) {
				array[j + i] = array[j + i + 1];
				array[j + i + 1] = "";
				//zmiana dla klucza
				arrayTemp[j + i] = arrayTemp[j + i + 1];
				arrayTemp[i + j + 1] = NULL;
				i++;
				if (j + i == arraySize - 1) {
					j = 0;
					i = 0;
				}
			}
			else {
				//WYJSCIE Z PETLI
				quit = true;
			}
		} while (quit == false);
		
		//wartosc nastepnej komorki ktora bedziemy badali
		int checkPosition = j + i + 1;
		//jesli pozycja badanej komorki jest na koncu tabeli to nastepna badana komorka bedzie komorka pierwsza (wracamy na poczatek tabeli)
		if (j == arraySize - 1) {
			checkPosition = 0;
		}
		//metoda ktora sprawdzi i zamieni(jesli bedzie trzeba) pozostale elementy o innych kluczach, aby znajdowaly sie w odpowiedniej komorce tabeli 
		checkArray(checkPosition);
	}

	void checkArray(int i) {
		
		bool quit = false;
		int counter = 0;
		int difference = 0;
	
		while(quit == false&&counter !=2){
			
			if (i+1==arraySize && array[i] == "" && array[0] == "") {
				quit = true;
			}
			else if (i + 1 == arraySize && array[i] == "" && array[0] != "") {
				i = 0;
				counter++;
			}
			else if (array[i] == "" && array[i + 1] == "") {
				quit = true;
			}
			else if (array[i] == "" && array[i + 1] != "") {
				i++;
			}
			else if(arrayTemp[i]%arraySize==i){
				i++;
				if (i == arraySize) {
					i = 0;
					counter++;
				}
			}
			else if (arrayTemp[i]%arraySize!=i) {
		
				key = arrayTemp[i];
				
				int z1 = arrayTemp[i] % arraySize;
				int z2 = i;
				z1 > z2 ? difference = z1 - z2 : difference = z2 - z1;
				int j = i + 1;
				int counter2 = 0;

				while (counter2 < arraySize) {

					if (arrayTemp[i] % arraySize == j) {
						if (array[j] == "") {
							arrayTemp[j] = arrayTemp[i];
							arrayTemp[i] = NULL;
							array[j] = array[i];
							array[i] = "";
							counter = 2;
							counter2 = arraySize;
							checkNext(j);
						}
					}
					else {
						j++;
						counter2++;
						if (j == arraySize) {
							j = 0;
						}
					}
				}
				}
			}
	}



	~Hashtable()
	{
		readFile.close();
		saveFile.close();
	}

};

int main()
{

	Hashtable hashtable;
	hashtable.createTest();

	return 0;
}