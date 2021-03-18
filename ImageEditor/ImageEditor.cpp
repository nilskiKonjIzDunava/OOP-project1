#include "ImageEditor.h"


ImageEditor::ImageEditor()
{
	first = nullptr;
	active = nullptr;
	numOfLayers = 0;
	activeBlue = activeGreen = activeRed = 0;
	name = "";
	width = height = leap = skip = 0;
}

ImageEditor::~ImageEditor()
{
	
	Layer* curr = nullptr;
	if (this == nullptr || this->first == nullptr)
		return;

	while (this->first != nullptr)
	{
		curr = this->first;
		first = first->next;
		
		for (int i = 0; (i < (curr->h)); i++)
		{
			delete[](curr->image[i]);
		}
		delete[]curr->image;
		curr->image = nullptr;
	}

	cout << "Broj slojeva je: " << this->numOfLayers;
}

// TREBA DODATI SLUCAJ DA JE NPR BMV=slika=.... neki ulsov kod 
//***************************************************************
//problem je ako nema imena, a karakter u niski je recimo k, sto treba da interpretiramo kao broj 107(dec),
// onda nam sledeci znak otkriva to, tj ako nema =, onda citamo sirinu, a ako ima, onda je pogresan format
bool ImageEditor::loadImage(unsigned char * image)
{
	// prvo treba proveriti da li je prethodno ucitana neka slika
	// ako jeste brisemo njene lejere i nju
	if (this->first != nullptr)
	{
		Layer* curr = nullptr;
		while (this->first != nullptr)
		{
			curr = this->first;
			first = first->next;

			for (int i = 0; (i < (curr->h)); i++)
			{
				delete[](curr->image[i]);
			}
			delete[]curr->image;
			curr->image = nullptr;
			curr->next = nullptr;
		}
		delete this->first;
		
	}


	int cnt = 0;
	string pomName="";


	if (image[cnt] == 'B')
		cnt++;
	else 
	{
		cout << "Pogresno formatiranje!" << endl;
		return false;
	}
	if (image[cnt] == 'M')
		cnt++;
	else
	{
		cout << "Pogresno formatiranje!" << endl;
		return false;
	}
	//ispravno BM

	// sada mora da dodje =name= 
	if (image[cnt] == '=')
	{
		cnt++; //pokazuje na prvi karakter name
		while (image[cnt] != '=')
		{
			pomName += image[cnt];
			cnt++;
		}
		name = pomName;
		cnt++;
		//ili u slucaju da je pozicija deljiva sa 4, onda citamo sirinu slike
		if (cnt % 4 == 0)
		{
			this->width = extractDim(image, cnt);
			this->height = extractDim(image, cnt+4);
			cnt += 8; // sad pokazuje na poslednji bajt visine
			
			
			
		}
		// ako pozicija nije deljiva sa 4 onda preskacemo polja do prve pozicije koja ispunjava uslov
		else 
		{
			do
			{
				cnt++;
				this->skip++;
			} while (cnt % 4 != 0);

			this->width = extractDim(image, cnt);                         //ovo uvuci u else
			this->height = extractDim(image, cnt + 4);
			cnt += 8; // sad pokazuje na poslednji bajt visine
			
		}
		
			
		
	}
	// *********************************************************************
	
	// ako nema imena slike
	else if (image[cnt + 1] != '=')
	{
		if (cnt % 4 == 0)
		{
			this->width = extractDim(image, cnt);
			this->height = extractDim(image, cnt + 4);
			cnt += 8; // sad pokazuje na poslednji bajt visine
		}
		else 
		{
			do
			{
				cnt++;
			} while (cnt % 4 != 0);

		}
		this->width = extractDim(image, cnt);
		this->height = extractDim(image, cnt + 4);
		cnt += 8; // sad pokazuje na poslednji bajt visine

	}
	else
	{
		cout << "Pogresno formatiranje!" << endl;
		return false;
	}
	
	// ako posle ucitanih dimenzija prva slobodna lokacija gde treba da budu info o pikselima nije delejiva sa 4, onda idemo do prve sledece koja jeste
	
	if (cnt % 4 != 0)
	{
		do
		{
			cnt++;
		} while (cnt % 4 != 0);

	}
	
	this-> leap = ((4 - (width * 3) % 4) % 4);

	// matrica praznih piksela
	Pixel** pixMatrix = new Pixel*[this->height];
	if (pixMatrix == nullptr)
	{
		cout << "Neuspesno alocirana memorija!" << endl;
		return false;
	}
	for (int i = 0; i < height; i++)
	{
		pixMatrix[i] = new Pixel[width];
		if (pixMatrix[i] == nullptr)
		{
			if (i == 0) 
				delete[] pixMatrix;
			else {
				for (int j = 0; j < i; j++) {
					delete[] pixMatrix[j];
				}
				delete[] pixMatrix;
			}
			cout << "Neuspesno alocirana memorija!" << endl;
			return false;
		}
	}
	// ako je sve uspesno alocirano onda dodeljujemo prvom u listi napravljeni lejer kao bazu i postavljamo opacity na 100
	first = new Layer(width, height, pixMatrix);
	first->opacity = 100;
	active = first;
	//numOfLayers++;
	// citamo info o pikselima iz ulaznog niza
	for (int i = height - 1; i >= 0; i--)
	{
		if (cnt % 4 == 0) 
		{
			for (int j = 0; j < width; j++)
			{
				first->image[i][j].b = image[cnt];
				first->image[i][j].g = image[cnt+1];
				first->image[i][j].r = image[cnt+2];
				first->image[i][j].noColor = 0;
				cnt += 3;

			}
		}
		else
		{
			cnt += leap;
			for (int j = 0; j < width; j++)
			{
				first->image[i][j].b = image[cnt];
				first->image[i][j].g = image[cnt + 1];
				first->image[i][j].r = image[cnt + 2];
				first->image[i][j].noColor = 0;
				cnt += 3;

			}

		}

		
					
	}
	bool testIfSucc = true;
	cout << "Matrica je lepo alocirana" << endl;
	return true;
}

Pixel ** ImageEditor::makeEmptyMatrix(int width, int height)
{
	Pixel** pixMatrix = new Pixel*[this->height];
	if (pixMatrix == nullptr)
	{
		cout << "Neuspesno alocirana memorija!" << endl;
		return nullptr;
	}
	for (int i = 0; i < height; i++)
	{
		pixMatrix[i] = new Pixel[width];
		if (pixMatrix[i] == nullptr)
		{
			if (i == 0)
				delete[] pixMatrix;
			else {
				for (int j = 0; j < i; j++) {
					delete[] pixMatrix[j];
				}
				delete[] pixMatrix;
			}
			cout << "Neuspesno alocirana memorija!" << endl;
			return nullptr;
		}
	}
	return pixMatrix;
}

Layer * ImageEditor::reverse_iterative(Layer * head)
{
	// ako ne postoji prvi (bazna slika) ili postoji samo bayna slika
	// onda ne treba menjati
	if (head == nullptr || head->next == nullptr) {
		return head;
	}

	Layer* list_to_do = head->next;

	Layer* reversed_list = head;
	reversed_list->next = nullptr;

	while (list_to_do != nullptr) {
		Layer* temp = list_to_do;
		list_to_do = list_to_do->next;

		temp->next = reversed_list;
		reversed_list = temp;
	}

	return reversed_list;
	
}

void ImageEditor::deallocateMatrix(Pixel ** pixi)
{
	for (int i = 0; i < this->height; i++)
	{
		delete[]pixi[i];
	}
	delete[]pixi;
	pixi = nullptr;
}



void ImageEditor::addLayer()
{
	Pixel** piksel = this->makeEmptyMatrix(this->width, this->height);
	Layer* newLyr = new Layer(width, height, piksel);
	Layer* curr = nullptr;
	Layer* after = nullptr;

	if (newLyr == nullptr)
	{
		cout << "Greska u alokaciji novog sloja!" << endl;
		return;
	}
	if (this->first == nullptr) {
		cout << "Greska, nema ucitane slike!" << endl;
		return;
	}
	else
	{
		curr = this->active;
		if (this->active->next != nullptr)
		{
			after = this->active->next;
			curr->next = newLyr;
			newLyr->next = after;
			this->active = active->next;
		}
		else
		{
			this->active->next = newLyr;
			this->active = active->next;
		}

		this->numOfLayers++;

	}
}

void ImageEditor::deleteLayer()
{
	if (numOfLayers == 0) {
		cout << "Greska, ne postoji ni jedan sloj preko osnovne slike!" << endl;
		return;
	}
	int h = this->height;
	int w = this->width;

	Layer* beforeAct = nullptr;
	beforeAct = first;
	Layer* targeted = nullptr;
	while (beforeAct->next != active)
		beforeAct = beforeAct->next;
		
	
	targeted = active;
	if (targeted->next != nullptr)
	{
		beforeAct->next = targeted->next;
		active = beforeAct;

	}
	else
	{
		active = beforeAct;
		active->next = nullptr;
	}

	// u svakom slucaju treba da se obrise targeted
	targeted->next = nullptr;
	for (int i = 0; i < h; i++)
	{
		delete[] targeted->image[i];
	}

	delete[] targeted->image;
	delete targeted;

	this->numOfLayers--;
}

void ImageEditor::selectLayer(int i)
{
	Layer* targeted = nullptr;

	if (this->numOfLayers < i)
	{
		cout << "Trazeni sloj ne postoji" << endl;
		return;
	}
	
	if (this->active != nullptr) {
		if (i < 0) {
			cout << "Greska, broj sloja mora biti pozitivan broj!" << endl; //PROVERI DA LI MOZE i=0 --> MOZE
			return;
		}
		else 
		{
			int j = i;
			targeted = this->first;
			
			while (j > 0 && targeted->next != nullptr)
			{                                
				targeted = targeted->next;                                             
				j--;
				//this->active = targeted;
			}
			//if (j == 0)
			//{
				this->active = targeted;
			//}
			//else
			//{
				//ako ima npr 3 lejera, a stavili smo i=5, ovo u slucaju da ne prodje ono sa proverom numOfLayers, KAD PROVERIS IZBRISI OVAJ AKO JE NEPOTREBNO
				cout << "Trazeni sloj ne postoji" << endl;
			//}
		}
	}
	else {
		cout << "Nema trenutno aktivnog sloja!" << endl;
	}
}

void ImageEditor::setLayerOpacity(int i)
{
	if (this->active == nullptr)
	{
		cout << "Greska, nema trenutno aktivnog sloja!" << endl;
		return;
	}

	if (i < 0 || i > 100) 
	{
		cout << "Greska, pogresan opseg za opacity!" << endl;
		return;
	}

	this->active->opacity = i;
}

void ImageEditor::invertColors()
{
	if (this->active == nullptr)
	{
		cout << "Greska, nema trenutno aktivnog sloja!" << endl;
		return;
	}
	
	for (int i = 0; i < this->height; i++) {
		for (int j = 0; j < this->width; j++) {
			this->active->image[i][j].b = 255 - this->active->image[i][j].b;
			this->active->image[i][j].g = 255 - this->active->image[i][j].g;
			this->active->image[i][j].r = 255 - this->active->image[i][j].r;
		}
	}
}

void ImageEditor::toGrayScale()
{
	unsigned char greyPix;

	if (this->active == nullptr)
	{
		cout << "Greska, nema trenutno aktivnog sloja!" << endl;
		return;
	}
	for (int i = 0; i < this->height; i++)
	{
		for (int j = 0; j < this->width; j++)
		{
			greyPix = (unsigned char)(0.3*(this->active->image[i][j].r)) + (unsigned char)(0.59*(this->active->image[i][j].g)) + (unsigned char)(0.11*(this->active->image[i][j].b));
			this->active->image[i][j].r = this->active->image[i][j].g = this->active->image[i][j].b = greyPix;
		}
		
	}
}

void ImageEditor::blur(int size)
{
	//zamućuje sve piksele aktivnog sloja tako što svaki piksel menja pikselom čiji su intenziteti boja dobijeni
	//kao srednje vrednosti intenziteta boja piksela koji se menja i svih piksela koji se nalaze oko piksela koji
	//se menja na udaljenosti od maksimalno size redova i size kolona.


	if (size <= 0) {
		cout << "Greska, udaljenost mora biti veca od 0!" << endl;
		return;
	}

	if (this->active == nullptr) {
		cout << "Greska, nema trenutno aktivnog sloja!" << endl;
		return;
	}
	
	// pomocna matrica gde po kojoj cemo da idemo i racunamo srednje vrednosti
	// potrebna je jer na osnovu originalne slike treba da blurujemo sliku
	Pixel** pomMatr= makeEmptyMatrix(this->width, this->height);      

	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			pomMatr[i][j].b = this-> active->image[i][j].b;
			pomMatr[i][j].g = this->active->image[i][j].g;
			pomMatr[i][j].r = this->active->image[i][j].r;
			pomMatr[i][j].noColor = this->active->image[i][j].noColor;
		}
	}

	int sumB=0, sumG=0, sumR=0, cnt=0;
	int x = 0; int y = 0;
	int sub_w, sub_h;
	for (int i = 0; i < this->height; i++)
	{
		for (int j = 0; j < this->width; j++)
		{
			if (this->active->image[i][j].noColor == 1) continue;
			sumB = sumG = sumR = 0;
			cnt = 0;
			x = i; y = j;
			for (sub_h = x - size; sub_h <= x + size; sub_h++)
			{
				for (sub_w = y - size; sub_w <= y + size; sub_w++)
				{
					if ((sub_w >= 0) && (sub_w < (this->width)) && (sub_h >= 0) && (sub_h < (this->height)) && (this->active->image[sub_h][sub_w].noColor == 0))
					{
						sumB += pomMatr[sub_h][sub_w].b;
						sumG += pomMatr[sub_h][sub_w].g;
						sumR += pomMatr[sub_h][sub_w].r;
						cnt++;
					}
				}
			}
			if (cnt == 0) continue;
			this->active->image[i][j].b = sumB / cnt;
			this->active->image[i][j].g = sumG / cnt;
			this->active->image[i][j].r = sumR / cnt;

		}

	}

	deallocateMatrix(pomMatr);

}

void ImageEditor::flipVertical()
{
	// ovde zakucamo kolonu i menjamo indekse vrsta
	// brojac po jednoj kolomi ide do pola
	
	Pixel pom;
	int h = this->height / 2;
	int w = this->width;
	if (this->active == nullptr) {
		cout << "Greska, nema trenutno aktivnog sloja!" << endl;
		return;
	}
	for (int i = 0; i < h; i++) 
	{
		for (int j = 0; j < w; j++)
		{                         
				pom = active->image[i][j];
				active->image[i][j] = active->image[height - i - 1][j];
				active->image[height - i - 1][j] = pom;
		}
		
	}
}

void ImageEditor::flipHorizontal()
{
	// zakucamo jednu vrstu i menjamo kolone
	if (this->active == nullptr) {
		cout << "Greska, nema trenutno aktivnog sloja!" << endl;
		return;
	}

	Pixel pom;
	int h = this->height;
	int w = this->width / 2;

	for (int i = 0; i < w; i++)
	{
		for (int j = 0; j < h; j++)
		{
			pom = active->image[j][i];
			active->image[j][i] = active->image[j][width - 1 - i];
			active->image[j][width - 1 - i] = pom;
		}
	}
}

void ImageEditor::crop(int x, int y, int w, int h)
{
	if (this->active == nullptr) {
		cout << "Greska, nema trenutno aktivnog sloja!" << endl;
		return;
	}
	if (x < 0 || y < 0 || w < 0 || h < 0 || x >= this->width || y >= this->height) {                                                                                  
		cout << "Greska, zadate kooordinate ili dimenzije za crop nisu ispravne!" << endl;
		return;
	}

	if ((x == 0) && (y == 0) && (w > this->width) && (h > this->height))
	{
		cout << "Greska, ne moze se iseci cela slika!" << endl;
		return;
	}

	if (w == 0 || h == 0) return;

	// treba azururati nove dimenzije nakon secenja
	// ako zadate dimenzije za secenje premasuju dimenzije slike, onda idemo dokle moze + azuriramo dimenzije
	int newWidth, newHeight;
	/*if (x + w <= this->width)
		newWidth = w;
	else
		newWidth = this->width - x;

	if (y + h <= this->height)
		newHeight = h;
	else
		newHeight = this->height - x;*/

	if (x + w - 1 < this->width) newWidth = w;
	if (x + w >= this->width) newWidth = this->width - x;
	if (y + h - 1 < this->height) newHeight = h;
	if (y + h >= this->height) newHeight = this->height - y;
	
	Pixel**pomMatr = makeEmptyMatrix(newWidth, newHeight);
	Layer* currLayer;
	currLayer = first;
	while (currLayer != nullptr) {

		// kopiramo u pomocnu matricu(koja je novih dimenzija) piksele koje zadrzavamo iz stare slike
		for (int i = y; i < y + newHeight; i++) 
		{
			for (int j = x; j < x + newWidth; j++) 
			{
				if (i <= this->height - 1 && j <= this->width - 1)
				{
					pomMatr[i - y][j - x].b = currLayer->image[i][j].b;
					pomMatr[i - y][j - x].g = currLayer->image[i][j].g;
					pomMatr[i - y][j - x].r = currLayer->image[i][j].r;
					pomMatr[i - y][j - x].noColor = currLayer->image[i][j].noColor;
				}
				
			}
		}
		// prolazimo starom matricom i postavljamo da je cela providna, u smislu nema boja (noColor=1)
		for (int i = 0; i < this->height; i++) 
		{
			for (int j = 0; j < this->width; j++)
			{
				currLayer->image[i][j].noColor = 1;
			}
		}

		// tako praznu matricu popunjavamo pikselima pomocne koja cuva kako ce izgledati slika posle kropovanja
		// ova i pretgodna petlja nisu potrebne ukoliko cu da insertujem novi lejer(pomMatr) umesto starog
		for (int i = 0; i < newHeight; i++) 
		{
			for (int j = 0; j < newWidth; j++)
			{
				currLayer->image[i][j].b = pomMatr[i][j].b;
				currLayer->image[i][j].g = pomMatr[i][j].g;
				currLayer->image[i][j].r = pomMatr[i][j].r;
				currLayer->image[i][j].noColor = pomMatr[i][j].noColor;
			}
		}

		currLayer = currLayer->next;
		
	}
	//brisemo staru matricu
	for (int i = 0; i < newHeight; i++)
	{
		delete[] pomMatr[i];
	}
	delete[] pomMatr;
	//pomMatr = nullptr;
	// azuriramo obavezno nove dimenzije image editora jer nismo brisali piksele, vec su ti visak pikseli oznaceni kao prazni
	this->width = newWidth;
	this->height = newHeight;
}

void ImageEditor::setActiveColor(string hex)
{
	// provera da li je prosledjena boja ispravno formatirana, ne znam da li je potrebno tho
	if (hex[0] != '#') {
		cout << "Greska, pogressan format boje!" << endl;
		return;
	}

	int l = hex.length();
	if (l != 7)
	{
		cout << "Greska, pogressan format boje!" << endl;
		return;
	}
	
	for (int i = 1; i < l; i++) 
	{
		if ((!(hex[i] >= '0' && hex[i] <= '9')) && (!(hex[i] >= 'A' && hex[i] <= 'F'))) 
		{

			cout << "Greska, pogressan format boje!" << endl;
			return;
		}
	}

	
	string hex_string = hex;
	string rr = hex_string.substr(1, 2);
	string gg = hex_string.substr(3, 2);
	string bb = hex_string.substr(5, 2);

	this->activeRed = stoul(rr, 0, 16); //stoul pretvara u long unsigned int
	this->activeGreen = stoul(gg, 0, 16);
	this->activeBlue = stoul(bb, 0, 16);

}

void ImageEditor::fillRect(int x, int y, int w, int h)
{
	if (this->active == nullptr) {
		cout << "Greska, nema trenutno aktivnog sloja!" << endl;
		return;
	}
	if (x < 0 || y < 0 || w < 0 || h < 0 || x >= this->width || y >= this->height)   //ovde granicni slicaj moze biti da je x=width-1, y=height-1, a w=h=0, da je to nda samo jedan piksel
	{                                                                                 // ISPITAJ KADA SU TAKO NA GRANICI DATE KOORDINATE!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!   
		cout << "Greska, zadate kooordinate ili dimenzije nisu ispravne!" << endl;
		return;
	}

	// AKO PIKSEL NE POSTOJI (NPR PRETJODNO JE KROPOVAN), KADA POZOVEMO FILL DA LI ZANEMARUJEMO TAJ PIKSEL ILI GA BOJIMO?
	// BOJIMO!!
	for (int i = 0; i < h; i++) {
		for (int j = 0; j < w; j++) {
			if (((y + i) < this->height) && ((x + j) < this->width))
			{
				this->active->image[y + i][x + j].noColor = 0;
				this->active->image[y + i][x + j].b = activeBlue;
				this->active->image[y + i][x + j].g = activeGreen;
				this->active->image[y + i][x + j].r = activeRed;
			}
		}
	}
}

void ImageEditor::eraseRect(int x, int y, int w, int h)  //DA LI SECEMO NA SVIM SLOJEVIMA ILI SAMO NA AKTIVNOM????? -->AKTIVNOM

{
	if (this->active == nullptr)
	{
		cout << "Greska, nema trenutno aktivnog sloja!" << endl;
		return;
	}
	if (x < 0 || y < 0 || w < 0 || h < 0 || x >= this->width || y >= this->height)
	{                                                                                    
		cout << "Greska, zadate kooordinate ili dimenzije nisu ispravne!" << endl;
		return;
	}

	if (this->active == this->first)
	{
		cout << "Greska, ne moze se obrisati pocetna slika!" << endl;
		return;
	}

	for (int i = 0; i < h; i++)
	{
		for (int j = 0; j < w; j++)
		{
			if ((y + i < this->height) && (x + j) < this->width) 
			{
				this->active->image[y + i][x + j].noColor = 1;                                
			}                                                                            
		}
	}
}



unsigned char * ImageEditor::saveImage()
{
	// DA LI DA RACUNAM IZLAZNU DUZINU???????????? mislim da moze
	// prvo alociramo prostor za izlaznu sliku
	//posto dimenzije slike mogu biti promenjeni jedino secenjem, i to se svi slojevi seku, imamo sacuvane dimenzije
	
	int sizeOfNewArr=0;
	int cnt=0;
	this->leap = ((4 - (width * 3) % 4) % 4);
	bool noNameflag = true;

	if (this->name.empty())
	{
		sizeOfNewArr = 2 + 2 + 8 + ((this->width)*(this->height)) * 3 + leap * (this->height - 1) + 1; // BM=2, ako nema naziva sigurno preskacemo 2 mesta, 8 za dimenzije
	}
	else
	{
		sizeOfNewArr = 2 + 2 + 8 + name.length() + skip + ((this->width)*(this->height)) * 3 + leap * (this->height - 1) + 1;
		noNameflag = false;
	}
	
	unsigned char* finalImage;
	finalImage = new unsigned char[sizeOfNewArr];

	finalImage[cnt] = 'B';
	cnt++;
	finalImage[cnt] = 'M';
	cnt++; // sad je snt na poziciji posle 'M'
	
	if (noNameflag)
	{
		//ako nema imena povecavamo cnt da bi imalo indeks niza od kog pocinje smestanje dimenzije, tj. pozicija 4
		cnt += 2;
	}
	else
	{
		finalImage[cnt] = '=';
		for (int i = 0; i < name.length(); i++)
			finalImage[cnt+1+i] = name[i];
		cnt += name.length()+1;
		finalImage[cnt] = '=';
		cnt += skip+1; //  moramo da dodjemo do pozicije deljive sa 4, sada je cnt na poziciji odakle treba da se unesu dimenzije
		
	}
	// smestanje dimezija
	finalImage[cnt] = (this->width) & 0x000000ff;
	cnt++;
	finalImage[cnt] = ((this->width) & 0x0000ff00)>>8;
	cnt++;
	finalImage[cnt] = ((this->width) & 0x00ff0000)>>16;
	cnt++;
	finalImage[cnt] = ((this->width) & 0xff000000)>>24;
	cnt++;

	finalImage[cnt] = (this->width) & 0x000000ff;
	cnt++;
	finalImage[cnt] = ((this->width) & 0x0000ff00) >> 8;
	cnt++;
	finalImage[cnt] = ((this->width) & 0x00ff0000) >> 16;
	cnt++;
	finalImage[cnt] = ((this->width) & 0xff000000) >> 24;
	

	// druga verzija
	/*finalImage[cnt] = this->width % 256;
	cnt++;
	finalImage[cnt] = (this->width - this->width % 256) / 256;
	cnt++;
	finalImage[cnt] = (this->width - finalImage[cnt-2] - finalImage[cnt-1] * 256) / (256 * 256);
	cnt++;
	finalImage[cnt] = (this->width - finalImage[cnt-3] - finalImage[cnt-2] * 256 - finalImage[cnt-1] * 256 * 256) / (256 * 256 * 256);
	cnt++;

	finalImage[cnt] = this->height % 256;
	cnt++;
	finalImage[cnt] = (this->height - this->height % 256) / 256;
	cnt++;
	finalImage[cnt] = (this->height - finalImage[cnt-2] - finalImage[cnt-1] * 256) / (256 * 256);
	cnt++;
	finalImage[cnt] = (this->height - finalImage[cnt-3] - finalImage[cnt-2] * 256 - finalImage[cnt-1]) * 256 * 256 / (256 * 256 * 256);
*/


	// posle 8 bajtova koliko zauzimaju dimenzije slike, sledeca pozicija ze sigurno biti deljiva sa 4, 
	// pa odmah moze smestanje piksela

	cnt++;

	// Imamp dve mogucnosti

	//1. Imamo samo baznu sliku:

	if (this->numOfLayers == 0)
	{
		for (int i = this->height - 1; i >= 0; i--)
		{
			for (int j = 0; j < this->width; j++)
			{
				if (this->active->image[i][j].noColor == 0) 
				{
					finalImage[cnt] = this->active->image[i][j].b;
					finalImage[cnt + 1] = this->active->image[i][j].g;
					finalImage[cnt + 2] = this->active->image[i][j].r;
				}
				else {
					finalImage[cnt] = finalImage[cnt + 1] = finalImage[cnt + 2] = 255;
				}
				cnt += 3;
			}
			while (cnt % 4 != 0) cnt++;
		}
		finalImage[cnt] = '\0';
		return finalImage;
	}
	// 2. ovde ako imamo vise lejera
	// treba nam pomocna izlazna matrica outImage
	else
	{
		outImagePixel** outImage = nullptr;
		outImage = new  outImagePixel*[this->height];
		if (outImage == nullptr)
		{
			cout << "Greska u alokaciji " << endl;
			return nullptr;
		}
		for (int j = 0; j < this->height; j++)
		{
			outImage[j] = new  outImagePixel[this->width];
			if (outImage[j] == nullptr)
			{
				if (j == 0) delete[] outImage;
				else
				{
					for (int k = 0; k < j; k++)
					{
						delete[] outImage[k];
					}
					delete[] outImage;
				}

			}
		}
		
		// za napravljenu izlaznu matricu stavljamo za leftPercentage 100

		for (int i = 0; i < this->height; i++)
		{
			for (int j = 0; j < this->width; j++)
			{
				outImage[i][j].b = 0;
				outImage[i][j].g = 0;
				outImage[i][j].r = 0;
				outImage[i][j].leftPercentage = 100;
			}
		}
		// treba da obrnemo listu kako bismo mogli da se krecemo od poslednje dodatog sloja ka baznoj slici

		this->first = reverse_iterative(this->first);         // NE ZABORAVI DA OPET OBRNES SLIKU PRE POVRTKA!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

		// pravimo matricu zbira prethodnih providnosti
		int** sumPrevOpacities = nullptr;
		sumPrevOpacities = new int*[this->height];
		for (int i = 0; i < this->height; i++) 
			sumPrevOpacities[i] = new int[this->width];

		for (int i = 0; i < this->height; i++) {
			for (int j = 0; j < this->width; j++) {
				sumPrevOpacities[i][j] = 0;
			}
		}
		Layer* currLayer = nullptr;
		currLayer = this->first; // sada pokazuje na poslednje dodat sloj, jer je lista obrnuta

		// idemo kroz sve lejere slike i za svaki racunamo koliki je udeo boje u izlaznoj slici svakog piksela svakog lejera 
		// taj udeo zavisi od procenta koji su lejeri iznad zauzeli (sumPrevOpacities), tj. koliko su propustili ti lejeri
		while (currLayer != nullptr)
		{
			for (int i = 0; i < this->height; i++)
			{
				for (int j = 0; j < this->width; j++)
				{
					if (outImage[i][j].leftPercentage != 0 && currLayer->image[i][j].noColor == 0)
					{
						if (outImage[i][j].leftPercentage >= (100 - sumPrevOpacities[i][j])*(currLayer->opacity) / 100)
						{
							outImage[i][j].b += (currLayer->image[i][j].b) * ((100 - sumPrevOpacities[i][j])*(double)(currLayer->opacity) / 10000);
							outImage[i][j].g += (currLayer->image[i][j].g) * ((100 - sumPrevOpacities[i][j])*(double)(currLayer->opacity) / 10000);
							outImage[i][j].r += (currLayer->image[i][j].r) * ((100 - sumPrevOpacities[i][j])*(double)(currLayer->opacity) / 10000);
							outImage[i][j].leftPercentage = outImage[i][j].leftPercentage - (100 - sumPrevOpacities[i][j])*(double)(currLayer->opacity) / 100;
							sumPrevOpacities[i][j] += (100 - sumPrevOpacities[i][j])*(currLayer->opacity) / 100;
						}
						else
						{
							outImage[i][j].b += currLayer->image[i][j].b *(double)outImage[i][j].leftPercentage / 100;
							outImage[i][j].g += currLayer->image[i][j].g *(double)outImage[i][j].leftPercentage / 100;
							outImage[i][j].r += currLayer->image[i][j].r *(double)outImage[i][j].leftPercentage / 100;
							sumPrevOpacities[i][j] = 100;
							outImage[i][j].leftPercentage = 0;

						}
					}
				}
			}
			currLayer = currLayer->next;

		}

		for (int i = 0; i < this->height; i++) delete[] sumPrevOpacities[i];
		delete[] sumPrevOpacities;
		sumPrevOpacities = nullptr;

		//treba smestiti info o pikselima u outImage matricu, cnt je na mestu odakle treba poceti
		for (int i = this->height - 1; i >= 0; i--)
		{
			for (int j = 0; j < this->width; j++)
			{
				//if (this->active->image[i][j].noColor == 0)
				//{
					finalImage[cnt] = outImage[i][j].b;
					finalImage[cnt + 1] = outImage[i][j].g;
					finalImage[cnt + 2] = outImage[i][j].r;
				//}
				/*else 
				{
					finalImage[cnt] = finalImage[cnt + 1] = finalImage[cnt + 2] = 255;
				}*/
				cnt += 3;
			}
			while (cnt % 4 != 0) cnt++;
		}
		for (int i = 0; i < this->height; i++) delete[] outImage[i];
		delete[] outImage;
		outImage = nullptr;

		
		finalImage[cnt] = '\0';
		this->first = reverse_iterative(this->first);
		return finalImage;


	}
		//return finalImage;
}

int ImageEditor::extractDim(unsigned char * niska, int pos)
{
	int dim = 0;
	for (int i = 0; i <= 3; i++)
	{
		
		dim = dim + (niska[pos + i]) *pow(16, 2 * i);
	}
	return dim;
}

 /*int ImageEditor::extractDim(unsigned char * niska, int pos)
{
	unsigned int dim = 0;

	for (int i = 3; i >=0; i++)
	{
		dim = (niska[pos + i] << 24) + (niska[pos + i]) << 16 + (niska[pos + i]) << 8 + niska[pos + i];
		
	}
	return dim;
}*/

//
//int ImageEditor::skipSpaces(unsigned char * niska, int i)
//{
//	for (; (i != '\0') && (niska[i] == ' '); i++);
//	return i;
//}


