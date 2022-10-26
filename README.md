# **MULTITHREADED PROXY-SERVER**
## _Proiect PSO : HTTP PROXY SERVER_
##
 Proiectul implica dezvoltarea unui program de tip Proxy-Server a carui sarcina este sa trimita mai departe, in numele clientului si catre serverul de origine al acestuia, request-uri de tip:
 * GET
 * POST
 * CONNECT 
 ##
 Server-ul trebuie sa poata gestiona mai multe solicitari simultane, de aceea se vor utiliza mai multe thread-uri pentru a trata in paralel request-urile de pe fiecare conexiune.
 ##
 ## _Planul de testare:_
 Pentru a testa functionalitatea serverului ne putem conecta direct din browser. Pentru a configura mai usor browser-ul sa acceseze serverul nostru vom utiliza extensia [FoxyProxy](https://getfoxyproxy.org/), in care vom adauga proxy-ul nostru: tip HTTP,pe adresa localhost: 0.0.0.0 , port 5555.
 ##
 Dupa ce va fi realizata conexiunea vom putea vizualiza toate request-urile in cadrul fisierului de log. Scrierea operatiilor este realizata de server dupa fiecare request. Accesul la fisierul unic de log se face controlat prin semafoare din cadrul conexiunilor multiple.
 ##
 Vom utiliza metoda HTTP CONNECT pentru a permite clientului din spate sa acceseze prin Proxy site-uri web ce utlizeaza SSL / TLS.
 ##
 Limbaj utilizat: C++
