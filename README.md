# **MULTITHREADED PROXY-SERVER**
## _Proiect PSO : HTTP PROXY SERVER_
 Proiectul implica dezvoltarea unui program de tip Proxy-Server, care sa functioneze ca un intermediar intre un client(browser web) si serverele pe care le acceseaza.
 Proxy-ul este conceput sa proceseze request-uri de tip:
 * POST
 * GET
 * CONNECT 
 ##
 Server-ul trebuie sa poata gestiona mai multe solicitari simultane, de aceea se vor utiliza mai multe thread-uri pentru a trata in paralel fiecare conexiune.
 ##
 Toate request-urile si raspunsurile pe care proxy-ul le proceseaza vor fi salvate in fisierul de proxy.log.Fiecare linie fisier respecta urmatorul format:
 *ID*: pentru fiecare conexiune va fi asignat un id, prin acest id putem identifica corespondenta dintre request si raspuns in fisierul de log
 *STATUS LINE
 *HOST
 *TIME

Scrierea in fisierul de log de catre fiecare thread va fi controlata printr-un semafor *logMutex* pentru a asigura consistenta fisierului
### Blocare trafic
 Proxy-ul poate bloca accesul la site-uri, site-urile blocate vor fi setate prin scriere in fisierul block.config a unei liste de host-uri.
 In cazul in care se incarca accesul la o pagina blocata se va trimite catre browser o pagina specifica de eroare (403 Forbidden).
 ## _Planul de testare:_
 Pentru a testa functionalitatea serverului ne putem conecta direct din browser. Pentru a configura mai usor browser-ul sa acceseze serverul nostru vom utiliza extensia [FoxyProxy](https://getfoxyproxy.org/), in care vom adauga proxy-ul nostru: tip HTTP,pe adresa localhost: 0.0.0.0 , port 5555.
 ##
 Vom utiliza metoda HTTP CONNECT pentru a permite clientului din spate sa acceseze prin Proxy site-uri web ce utlizeaza SSL / TLS.
 ##
 Limbaj utilizat: C++
