**COIN RECOGNITION - OPENCV**

## ELKÉPZELÉS

#### [1. Bemeneti eszköz használata](#i.-bemeneti-eszköz-használata)

#### [2. Képjavítás](#ii.-képjavitás)

#### [3. Szabályos körök keresése](#iii.-szabályos-körök-keresése)

#### [4. Vizsgálat (érme-e, amit körként detektáltunk?)](#iv.-vizsgálat)

#### [5. Értékfelismerés (hány forintos érme?)](#v.-értékfelismerés)



## I. BEMENETI ESZKÖZ HASZNÁLATA

- Mintha egy képet olvasnánk be
    - Csak most éppen 30 darabot másodpercenként
    - Ezáltal teljesítménycsökkenés
- A program futásának ideje a periféria elérhetősége függvényében

```cpp
Mat Frame;
VideoCapture cap(0); // alapértelmezett kamerabemenet

if (!cap.isOpened())
{
  cout << "Can't open camera."
}

while (true)
{
  cap >> Frame;
  imshow("Display window - Original", Frame)
  //
  //További programkód helye
  //

  char c = (char)waitKey(25);
  if((c == 27) or (!cap.isOpened()))
  {
    break;
  }
}
```

## II. KÉPJAVÍTÁS

- Szürkeárnyalattá alakítjuk át
- Egyszerű Median Blur
- Visszaadjuk a kapott képet

```cpp
Mat src = Frame;
Mat gray;
cvtColor(src, gray, COLOR_BGR2GRAY);
medianBlur(gray, gray, 5);
return gray;
```

## III./I. SZABÁLYOS KÖRÖK KERESÉSE

Hough transzformáció egyik speciális esetét használjuk

```cpp
vector<Vec3f> circles;
HoughCircles(gray, circles, HOUGH_GRADIENT, 1, gray.rows / 16, 100, 30, 1, 100 ); // utolsó kettő felel a körök minimum és maximum sugarának
return circles;
```

## III./II.HOUGH CIRCLE TRANSFORM (HCT)

- Kontúr alapú éldetektálást végzünk
- Alapvetően Hough transzformációnál explicit görbéket keresünk
    - Ilyen alakban: F(x, y, a 1 , a 2 , a 3 .... an) = 0
- Körök esetén 3 dimenziós Hough-tér lesz
    - Mivel az általános kör képlete x^2 + y^2 = r^2
    - Ezt átírva: F(x, y, r) = ax^2 + by^2 - r^2 = 0
    - De ez túl erőforrás igényes, így át tudjuk alakítani 2 dimenziósra, ha egy adott méretű kört keresünk →min és maxértéket megadva végig nézzük őket

## III./III. HOGYAN HASZNÁLJUK FEL A HCT-OT?

A HCT által kinyert kör mentén kimetsszük az érmét és eltároljuk

```cpp
if (circles.size() != 0)
{
	Vec3f circ = circles[0];
	Mat1b mask(src.size(), uchar(0));
	circle(mask, Point(circ[0], circ[1]), circ[2], Scalar(255), FILLED);
	Rect bbox(circ[0] - circ[2], circ[1] - circ[2], 2 * circ[2], 2 * circ[2]);
	Mat3b res(src.size(), Vec3b(0, 0, 0));
	src.copyTo(res, mask);
	res = res(bbox);
	return res;
}
else
{
	Mat image(240, 240, CV_8UC3, Scalar(0, 0, 0));
	return image;
}
```

### ÉRDEKESSÉG

Detektál körök megjelenítése, rámaszkolva az eredetire

```cpp
Vec3i c = circles[i];
Point center = Point(c[0], c[1]);
// kör közepe
circle(src, center, 1, Scalar(0, 100, 100), 3, LINE_AA);
// kör körvonala
int radius = c[2];
circle(src, center, radius, Scalar(255, 0, 255), 3, LINE_AA);
```

## VIZSGÁLAT

- Mintakeresési algoritmus
- Pixelenkénti összehasonlítás
- Visszatérünk egy százalék értékkel

```cpp
Mat scoreImg;
double maxScore;
matchTemplate(coin, Crop(), scoreImg, TM_SQDIFF_NORMED);
minMaxLoc(scoreImg, 0, &maxScore);
//cout << (1 - maxScore) << endl;
return 1 - maxScore;
```

## ÉRTÉKFELISMERÉS

- Százalékalapú
    - (50% alatt nem fogadjuk el)

```cpp
vector<double> percent;
percent.push_back(match(_5));
percent.push_back(match(_10));
//...

double sum = 0;
for (int i = 0; i < 6; i++)
{		
  sum += (percent[i]);
}
if (sum != 0)
{
	if ((1 - match(_5) == *max_element(percent.begin(), percent.end())) && *max_element(percent.begin(), percent.end()) > 0.5)
	{
		cout << "5Ft - " << *max_element(percent.begin(), percent.end()) * 100 << '%' << endl;
	}
	if ((1 - match(_10) == *max_element(percent.begin(), percent.end())) && *max_element(percent.begin(), percent.end()) > 0.5)
	{
		cout << "10Ft - " << *max_element(percent.begin(), percent.end()) * 100 << '%' << endl;
	}
  //...
}
```
