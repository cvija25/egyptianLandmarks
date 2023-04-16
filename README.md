# egyptianLandmarks

Zdravo svima, u sledećem tekstu ću opisati svoj projekat iz Računarske grafike radjen školske 2022-23

Scena predstavlja vid nekih znamenitosti iz starog Egipta u 3D svetu OpenGL-a. 

Implementirane oblasti: osnovne oblasti (1-8 nedelja), Cubemaps/Skybox (grupa A), Point shadows(grupa B)

Video reprezentacija: https://youtu.be/5Iq1pMNhvTc

Kao sto možemo da vidimo izvor svetlosti je Point svetlo koje radi po Blinn Phong-ovom modelu, u obliku je male piramide koja liči na beli vrh najveće 
piramide na sceni, ono može da se kontroliše dugmićima na tastaturi na sledeći način:  

I - pomera se napred

K - nazad

J - levo

L - desno

Y - gore

H - dole

Možemo da primetimo kako se senke pomeraju pomeranjem svetla. Jos neki dugmići koji su u funkciji jesu M kojim otkljucavamo miš i možemo da pomerimo prozor ili da ga uveličamo, ESC da zatvorimo prozor, WASD za pomeranje kamere, pomeranjem miša rotiramo istu, scroll na misu uveličava i smanjuje.

Blending oblast je pokrivena koristeći discard metodu pri lepljenju hijeroglifa (transparentnih tekstura) na zid jednog od modela.
Face Culling je primenjen na svetlu.

SkyBox(HDRI) je preuzet sa sajta: https://polyhaven.com/

Modeli: https://www.cgtrader.com/

Transparentne teksture: https://pngtree.com/
