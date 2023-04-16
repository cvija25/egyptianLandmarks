# egyptianLandmarks

Zdravo svima, u sledećem tekstu ću opisati svoj projekat iz Računarske grafike radjen školske 2022-23

Scena predstavjla vid nekih znamenitosti iz starog Egipta u 3D svetu OpenGL-a. 

Implementirane oblasti: osnovne oblasti (1-8 nedelja), Cubemaps/Skybox (grupa A), Point shadows(grupa B)

Kao sto mozemo da vidimo izvor svetlosti je Point svetlo koje radi po Blinn Phong-ovom modelu, u obliku je male piramide koja lici na beli vrh najvece piramide na sceni, ono moze da se kontrolise dugmicima na tastaturi na sledeci nacin:  I - pomera se napred
                                                                                       K - nazad
                                                                                       J - levo
                                                                                       L - desno
                                                                                       Y - gore
                                                                                       H - dole
Mozemo da primeteimo kako se senke pomeraju pomeranjem svetla. Jos neki dugmici koji su u funkciji jestu M kojim otkljucavamo mis i mozemo da pomerimo prozor ili uvelicamo, ESC da zatvorimo prozor, WASD za pomeranje kamere i pomeranjem misa rotiramo istu, scroll na misu uvelicava i smanjuje.

Blending oblast je pokrivena koristeci discard metodu pri lepljenju hijeroglifa (transparentnih tekstura) na zid jednog od modela.
Face Culling je primenjen na svetlu.

SkyBox(HDRI) je preuzet sa sajta: https://polyhaven.com/
Modeli: https://www.cgtrader.com/
Transparentne teksture: https://pngtree.com/

Video reprezentacija: https://youtu.be/5Iq1pMNhvTc
