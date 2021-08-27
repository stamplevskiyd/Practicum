#include <iostream>

#include <string>

#include <cstdio>

#include <cstring>

#include <cstdlib>

//Beta-5. Добавлено переопределение вывода, меню и тестовый режим, поправлен вывод.

//Реализован весь нужный функционал. Предварительно-готовая программа

#define L 10

#define N 10

void MemoryError()

{

std::cout << "Error!" << std::endl;

exit(1);

}

char* Strcpy(char *Result)

{

char* Base;

Base = new(std::nothrow) char[strlen(Result) + 1]; //под '\0'

if (Base == NULL)

MemoryError();

for (int i = 0; i < strlen(Result); i++)

Base[i] = Result[i];

Base[strlen(Result)] = '\0';

return Base;

}

char* InitString()

{

char c;

char *String = new(std::nothrow) char[L];

if (String == NULL)

MemoryError();

int Current = 0, Length = L;

c = getchar();

while ( (c != '\n') && (c != '\0') )

{

String[Current] = c;

Current++;

c = getchar();

if (Current == Length) //если дальше писать некуда

{

Length += L;

char *Additional = new(std::nothrow) char[Length];

if (Additional == NULL)

MemoryError();

int i;

for (i = 0; i < Current; i++)

Additional[i] = String[i];

delete[] String;

String = new (std::nothrow) char[Length];

if (String == NULL)

MemoryError();

for (i = 0; i < Current; i++)

String[i] = Additional[i];

delete[] Additional;

}

}

String[Current] = '\0'; //символ конца строки. Написано так, что Current никогда не выйдет за границы массива

return String;

}

class Item{

protected:

float Size[3];

int Price;

char *Color, *Name;

static int SumPrice;

public:

Item() : Price(0), Color(NULL), Name(NULL){ Size[0] = Size[1] = Size[2] = 0;}

virtual void Init() = 0;

virtual ~Item(){delete[] Color; delete[] Name;}

static int ShowSumPrice(){ return SumPrice;}

};

int Item::SumPrice; //суммарная цена всех созданных объектов

class Lamp: public Item{

int Brightness;

int PowerConsumption;

public:

void Init();

Lamp();

Lamp(float, float, float, int, char*, char*, int, int);

Lamp(const Lamp&);

Lamp& operator=(Lamp);

~Lamp();

friend std::ostream& operator<< (std::ostream &out, const Lamp&);

};

class Sofa: public Item{

int SeatCount; //число сидений

char* Material;

bool Folding; //раскладывается или нет

public:

void Init();

Sofa();

Sofa(float, float, float, int, char*, char*, int, char*, bool);

Sofa(const Sofa&);

Sofa& operator=(Sofa A);

~Sofa();

friend std::ostream& operator<< (std::ostream &out, const Sofa&);

};

class Cupboard: public Item{ //шкаф

int ShelfCount;

int DoorCount;

char *Material;

public:

void Init();

Cupboard();

Cupboard(float, float, float, int, char*, char*, int, int, char*);

Cupboard(const Cupboard&);

Cupboard& operator=(const Cupboard&);

~Cupboard();

friend std::ostream& operator<< (std::ostream &out, const Cupboard&);

};

class TV: public Item {

float ScreenSize;

int PowerConsumption;

bool SmartTV;

public:

void Init();

TV();

TV(float, float, float, int, char*, char*, float, int, bool);

TV(const TV&);

TV& operator= (const TV&);

~TV();

friend std::ostream& operator<< (std::ostream &out, const TV&);

};

///////////////////////////////////////////////////////////////////////////////////////

Lamp::Lamp()

{

Brightness = 0;

PowerConsumption = 0;

Color = NULL;

Name = NULL;

Price = 0;

SumPrice += Price;

}

Lamp::Lamp(float x, float y, float z, int LampPrice, char* LampColor, char* LampName, int BrightnessLevel, int Power)

{

Size[0] = x;

Size[1] = y;

Size[2] = z;

Price = LampPrice;

SumPrice += Price;

Brightness = BrightnessLevel;

PowerConsumption = Power;

Color = Strcpy(LampColor);

Name = Strcpy(LampName);

}

Lamp::Lamp(const Lamp& A)

{

Size[0] = A.Size[0];

Size[1] = A.Size[1];

Size[2] = A.Size[2];

Price = A.Price;

SumPrice += Price;

Brightness = A.Brightness;

PowerConsumption = A.PowerConsumption;

Color = Strcpy(A.Color);

Name = Strcpy(A.Name);

}

void Lamp::Init()

{

std::cout << "Write lamp model:" << std::endl;

this->Name = InitString();

std::cout << "Write maximal brightness level(lux):" << std::endl;

std::cin >> this->Brightness;

while (std::cin.get() != '\n'); //отсеивает лишние символы после пробела

std::cout << "Write maximal power consumption level(watt):" << std::endl;

std::cin >> this->PowerConsumption;

while (std::cin.get() != '\n');

std::cout << "Write size (cm*cm*cm):" << std::endl;

std::cin >> this->Size[0]; std::cin >> this->Size[1]; std::cin >> this->Size[2];

while (std::cin.get() != '\n');

std::cout << "Write color:" << std::endl;

this->Color = InitString();

int OldPrice = this->Price;

std::cout << "Write price ($):" << std::endl;

std::cin >> this->Price;

SumPrice += (Price - OldPrice);

}

Lamp& Lamp::operator=(Lamp A)

{

Size[0] = A.Size[0];

Size[1] = A.Size[1];

Size[2] = A.Size[2];

SumPrice -= Price;

SumPrice += A.Price;

Price = A.Price;

Brightness = A.Brightness;

PowerConsumption = A.PowerConsumption;

Color = Strcpy(A.Color);

Name = (A.Name);

return *this;

}

Lamp::~Lamp()

{

//вообще-то он не нужен, вся динамика содержится в базовом классе. Только для проверки

//std::cout << "Lamp destr" << std::endl;

}

std::ostream &operator<<(std::ostream &out, const Lamp &A)

{

std::cout << "Model: " << A.Name << std::endl;

std::cout << "Characteristics:" << std::endl;

std::cout << "Maximal brightness level(lux): " << A.Brightness << std::endl;

std::cout << "Maximal power consumption level(watt): " << A.PowerConsumption << std::endl;

std::cout << "Size (cm*cm*cm): " << A.Size[0] << "x" << A.Size[1] << "x" << A.Size[2] <<std::endl;

std::cout << "Color: " << A.Color << std::endl;

std::cout << "Price: " << A.Price << " $"<< std::endl;

//std::cout << std::endl;

return out;

}

/////////////////////////////////////////////////////////////////////////////////////////////

Sofa::Sofa()

{

SeatCount = 0;

Folding = false;

Material = NULL;

}

Sofa::Sofa(float x, float y, float z, int SofaPrice, char* SofaColor, char* SofaName, int SofaSeatCount, char* SofaMaterial, bool SofaFolding)

{

Size[0] = x;

Size[1] = y;

Size[2] = z;

Price = SofaPrice;

SumPrice += Price;

SeatCount = SofaSeatCount;

Name = Strcpy(SofaName);

Color = Strcpy(SofaColor);

Material = Strcpy(SofaMaterial);

SumPrice += Price;

Folding = SofaFolding;

}

Sofa::Sofa(const Sofa& A)

{

Size[0] = A.Size[0];

Size[1] = A.Size[1];

Size[2] = A.Size[2];

Price = A.Price;

SeatCount = A.SeatCount;

Folding = A.Folding;

Color = Strcpy(A.Color);

Name = Strcpy(A.Name);

Material = Strcpy(A.Material);

SumPrice += Price;

}

void Sofa::Init()

{

std::cout << "Write sofa model:" << std::endl;

this->Name = InitString();

std::cout << "Write seats count:" << std::endl;

std::cin >> this->SeatCount;

while (std::cin.get() != '\n'); //отсеивает лишние символы после пробела

std::cout << "Does the sofa folds? Yes/No:" << std::endl;

std::string Foldable;

std::cin >> Foldable;

bool Mistake = true;

while (Mistake)

{

Mistake = false;

if (Foldable == "Yes")

this->Folding = true;

else

if (Foldable == "No")

this->Folding = false;

else

{

Mistake = true;

std::cout << "Wrong input. Try again" << std::endl;

std::cin >> Foldable;

}

}

std::cout << "Write size (cm*cm*cm):" << std::endl;

std::cin >> this->Size[0]; std::cin >> this->Size[1]; std::cin >> this->Size[2];

while (std::cin.get() != '\n');

std::cout << "Write color:" << std::endl;

this->Color = InitString();

std::cout << "Write material:" << std::endl;

this->Material = InitString();

int OldPrice = this->Price;

std::cout << "Write price($):" << std::endl;

std::cin >> this->Price;

SumPrice += (Price - OldPrice);

}

Sofa& Sofa::operator= (Sofa A)

{

Size[0] = A.Size[0];

Size[1] = A.Size[1];

Size[2] = A.Size[2];

SumPrice -= Price;

SumPrice += A.Price;

Price = A.Price;

SeatCount = A.SeatCount;

Folding = A.Folding;

Color = Strcpy(A.Color);

Name = Strcpy(A.Name);

Material = Strcpy(A.Material);

return *this;

}

Sofa::~Sofa()

{

//std::cout << "Sofa destr" << std::endl;

delete[] Material;

}

std::ostream& operator<< (std::ostream &out, const Sofa &A)

{

std::cout << "Model: " << A.Name << std::endl;

std::cout << "Characteristics:" << std::endl;

std::cout << "Count of seats: " << A.SeatCount << std::endl;

std::cout << "Material: " << A.Material << std::endl;

if (A.Folding)

std::cout << "This sofa is folding" << std::endl;

else

std::cout << "This sofa is not folding" << std::endl;

std::cout << "Size (cm*cm*cm): " << A.Size[0] << "x" << A.Size[1] << "x" << A.Size[2] <<std::endl;

std::cout << "Color: " << A.Color << std::endl;

std::cout << "Price: " <<" $" << A.Price << std::endl;

//std::cout << std::endl;

return out;

}

////////////////////////////////////////////////////////////////////////////////////////////////

Cupboard::Cupboard()

{

ShelfCount = 0;

DoorCount = 0;

Material = NULL;

}

Cupboard::Cupboard(float x, float y, float z, int CupboardPrice, char *CupboardColor, char *CupboardName, int CupboardShelfCount, int CupboardDoorCount, char* CupboardMaterial)

{

Size[0] = x;

Size[1] = y;

Size[2] = z;

Price = CupboardPrice;

SumPrice += Price;

Color = Strcpy(CupboardColor);

Name = Strcpy(CupboardName);

Material = Strcpy(CupboardMaterial);

ShelfCount = CupboardShelfCount;

DoorCount = CupboardDoorCount;

}

Cupboard::Cupboard(const Cupboard &A)

{

Size[0] = A.Size[0];

Size[1] = A.Size[1];

Size[2] = A.Size[2];

Price = A.Price;

SumPrice += Price;

Color = Strcpy(A.Color);

Name = Strcpy(A.Name);

Material = Strcpy(A.Material);

DoorCount = A.DoorCount;

ShelfCount = A.ShelfCount;

}

void Cupboard::Init()

{

std::cout << "Write cupboard model:" << std::endl;

this->Name = InitString();

std::cout << "Write count of shelves:" << std::endl;

std::cin >> this->ShelfCount;

while (std::cin.get() != '\n'); //отсеивает лишние символы после пробела

std::cout << "Write count of doors:" << std::endl;

std::cin >> this->DoorCount;

while (std::cin.get() != '\n');

std::cout << "Write size (cm*cm*cm):" << std::endl;

std::cin >> this->Size[0]; std::cin >> this->Size[1]; std::cin >> this->Size[2];

while (std::cin.get() != '\n');

std::cout << "Write color:" << std::endl;

this->Color = InitString();

int OldPrice = this->Price;

std::cout << "Write material" << std::endl;

this->Material = InitString();

std::cout << "Write price($):" << std::endl;

std::cin >> this->Price;

SumPrice += (Price - OldPrice);

}

Cupboard& Cupboard::operator= (const Cupboard& A)

{

Size[0] = A.Size[0];

Size[1] = A.Size[1];

Size[2] = A.Size[2];

Price = A.Price;

SumPrice += Price;

Color = Strcpy(A.Color);

Name = Strcpy(A.Name);

Material = Strcpy(A.Material);

DoorCount = A.DoorCount;

ShelfCount = A.ShelfCount;

return *this;

}

Cupboard::~Cupboard()

{

//std::cout << "Cupboard destr" << std::endl;

delete[] Material;

}

std::ostream& operator<< (std::ostream &out, const Cupboard &A)

{

std::cout << "Model: " << A.Name << std::endl;

std::cout << "Characteristics:" << std::endl;

std::cout << "Count of shelves: " << A.ShelfCount << std::endl;

std::cout << "Count of doors: " << A.DoorCount << std::endl;

std::cout << "Size (cm*cm*cm): " << A.Size[0] << "x" << A.Size[1] << "x" << A.Size[2] <<std::endl;

std::cout << "Color: " << A.Color << std::endl;

std::cout << "Material: " << A.Material << std::endl;

std::cout << "Price: "<< A.Price << " $" <<std::endl;

//std::cout << std::endl;

return out;

}

//////////////////////////////////////////////////////////////////////////////////////

TV::TV()

{

ScreenSize = 0;

PowerConsumption = 0;

SmartTV = false;

}

TV::TV(float x, float y, float z, int TVPrice, char* TVName, char* TVColor, float Screen, int Power, bool Smart)

{

Size[0] = x;

Size[1] = y;

Size[2] = z;

Price = TVPrice;

Color = Strcpy(TVColor);

Name = Strcpy(TVName);

ScreenSize = Screen;

PowerConsumption = Power;

SmartTV = Smart;

}

TV::TV(const TV &A)

{

Size[0] = A.Size[0];

Size[1] = A.Size[1];

Size[2] = A.Size[2];

Price = A.Price;

ScreenSize = A.ScreenSize;

PowerConsumption = A.PowerConsumption;

SmartTV = A.SmartTV;

Color = Strcpy(A.Color);

Name = Strcpy(A.Name);

SumPrice += Price;

}

void TV::Init()

{

std::cout << "Write TV model:" << std::endl;

this->Name = InitString();

std::cout << "Write screen size (inches):" << std::endl;

std::cin >> this->ScreenSize;

while (std::cin.get() != '\n'); //отсеивает лишние символы после пробела

std::cout << "Write maximal power consumption level(watt):" << std::endl;

std::cin >> this->PowerConsumption;

while (std::cin.get() != '\n');

std::cout << "Write size (cm*cm*cm):" << std::endl;

std::cin >> this->Size[0]; std::cin >> this->Size[1]; std::cin >> this->Size[2];

while (std::cin.get() != '\n');

std::cout << "Write color:" << std::endl;

this->Color = InitString();

int OldPrice = this->Price;

std::cout << "Is this TV a smart TV Yes/No?" << std::endl;

std::string Smart;

std::cin >> Smart;

bool Mistake = true;

while (Mistake)

{

Mistake = false;

if (Smart == "Yes")

this->SmartTV = true;

else

if (Smart == "No")

this->SmartTV = false;

else

{

Mistake = true;

std::cout << "Wrong input. Try again" << std::endl;

std::cin >> Smart;

}

}

std::cout << "Write price:" << std::endl;

std::cin >> this->Price;

SumPrice += (Price - OldPrice);

}

TV& TV::operator=(const TV& A)

{

Size[0] = A.Size[0];

Size[1] = A.Size[1];

Size[2] = A.Size[2];

Price = A.Price;

ScreenSize = A.ScreenSize;

PowerConsumption = A.PowerConsumption;

SmartTV = A.SmartTV;

Color = Strcpy(A.Color);

Name = Strcpy(A.Name);

SumPrice += Price;

return *this;

}

TV::~TV()

{

//std::cout << "TV destr" << std::endl;

}

std::ostream& operator<< (std::ostream &out, const TV &A)

{

std::cout << "Model: " << A.Name << std::endl;

std::cout << "Characteristics:" << std::endl;

std::cout << "Screen size (inches): " << A.ScreenSize << std::endl;

if (A.SmartTV)

std::cout << "This TV has smart functions" << std::endl;

else

std::cout << "This TV has no smart functions" << std::endl;

std::cout << "Maximal power consumption level(watt): " << A.PowerConsumption << std::endl;

std::cout << "Size (cm*cm*cm): " << A.Size[0] << "x" << A.Size[1] << "x" << A.Size[2] <<std::endl;

std::cout << "Color: " << A.Color << std::endl;

std::cout << "Price($): " << A.Price << std::endl;

//std::cout << std::endl;

return out;

}

///////////////////////////////////////////////////////////////////////////

int main()

{

bool Repeat = true;

int Mode;

Lamp LampArray[N]; //можно и динамические сделать, но уж слишком муторно и не имеет особого смысла

Sofa SofaArray[N];

Cupboard CupboardArray[N];

TV TVArray[N];

int LampCount = 0, SofaCount = 0, CupboardCount = 0, TVCount = 0;

while (Repeat)

{

Repeat = false;

std::cout << "What goods do you want to add to cart?" << std::endl;

std::cout << "1 - Lamp" << std::endl;

std::cout << "2 - Sofa" << std::endl;

std::cout << "3 - Cupboard" << std::endl;

std::cout << "4 - TV" << std::endl;

std::cin >> Mode;

while (std::cin.get() != '\n');

switch(Mode)

{

case 1:

if (LampCount == N)

std::cout << "Error! Too much lamps in cart" << std::endl;

else

{

LampArray[LampCount].Init();

LampCount++;

}

break;

case 2:

if (SofaCount == N)

std::cout << "Error! Too much sofas in cart" << std::endl;

else

{

SofaArray[SofaCount].Init();

SofaCount++;

}

break;

case 3:

if (CupboardCount == N)

std::cout << "Error! Too much cupboards in cart" << std::endl;

else

{

CupboardArray[CupboardCount].Init();

CupboardCount++;

}

break;

case 4:

if (TVCount == N)

std::cout << "Error! Too much TVs in cart" << std::endl;

else

{

TVArray[TVCount].Init();

TVCount++;

}

break;

case 111:

//test mode.

{

Lamp TestLamp1, TestLamp2;

std::cout << "Enter first lamp" << std::endl;

TestLamp1.Init();

TestLamp2 = TestLamp2;

Lamp TestLamp3(TestLamp1);

std::cout << "operator=" << std::endl;

std::cout << TestLamp2 << std::endl;

std::cout << "copy constructor" << std::endl;

std::cout << TestLamp3 << std::endl;

}

default:

std::cout << "Wrong input. Try again" << std::endl;

Repeat = true;

break;

}

if (!Repeat) //если не выпали на ошибку

{

bool CorrectAnswer = false;

std::string Answer;

std::cout << "Do you want to add new items? Yes/No" << std::endl;

if (!CorrectAnswer)

{

std::cin >> Answer;

if (Answer == "Yes")

{

Repeat = true;

CorrectAnswer = true;

}

else

if (Answer == "No") //а если ответ ни Yes, ни No, ничего не меняем и идем заново

{

Repeat = false;

CorrectAnswer = true;

}

else

std::cout << "Wrong input. Try again" << std::endl;

}

}

}

std::cout << "Total cart:" << std::endl;

int i;

if (LampCount != 0)

{

std::cout << "Lamps" << std::endl;

for (i = 0; i < LampCount; i++)

std::cout << LampArray[i] << std::endl;

}

if (SofaCount != 0)

{

std::cout << "Sofas" << std::endl;

for (i = 0; i < SofaCount; i++)

std::cout << SofaArray[i] << std::endl;

}

if (CupboardCount != 0)

{

std::cout << "Cupboards" << std::endl;

for (i = 0; i < CupboardCount; i++)

std::cout << CupboardArray[i] << std::endl;

}

if (LampCount != 0)

{

std::cout << "TVs" << std::endl;

for (i = 0; i < TVCount; i++)

std::cout << TVArray[i] << std::endl;

}

std::cout << "Summary bill: " << Lamp::ShowSumPrice() << "$" << std::endl;

return 0;

}
