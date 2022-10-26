# Team-PSO
Proiect PSO : HTTP PROXY

Proiectul implica dezvoltarea unei aplicatii de tip Proxy-Server.
Serverul este capabil sa transmita mai departe, in numele clientului, request-uri HTTP de tip:GET,POST,CONNECT.
Sunt suportate mai multe conexiuni simultane, se utilizeaza mai multe thread-uri pentru a trata in paralel request-urile de pe fiecare conexiune.

Pentru a testa functionalitatea serverului ne putem conecta direct din browser.Pentru a configura mai usor browser-ul sa acceseze serverul nostru
vom utiliza extensia FoxyProxy, in care vom adauga Proxy-ul nostru:tip HTTP, pe adresa localhost 0.0.0.0, port 5555.
Dupa ce facem activa conexiunea la proxy vom putea vizualiza toate request-urile in cadrul fisierului de log.Serverul are ca functionalitate scrierea
in fisierul de log a operatiilor dupa fiecare request.Accesul la fisierul unic de log se face controlat prin semafoare din cadrul conexiunilor multiple.

Vom utiliza metoda HTTP CONNECT pentru a permite clientului din spate sa acceseze prin Proxy site-uri web ce utilizeaza SSL/TLS.
