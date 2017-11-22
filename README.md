# Manual

**UWAGA:** ​ Serwer pracuje na porcie 27015, jeżeli z jakiegoś powodu nie odpowiada ci ten
port należy go zmienić w kodzie źródłowym zarówno po stronie serwera jak i klienta.
Linia za to odpowiedzialna to :
#define DEFAULT_PORT "27015"

## Opis ogólny:

Program bazuje na architekturze klient-serwer w której klientem jest kupujący natomiast
serwerem jest sprzedawca. Sprzedawca ma specjalny atrybut czapkę , którą to początkowo
nosi na głowie. Klient może zamówić banana lub pomarańczę. Gdy sprzedawca ma
założoną czapkę i klient poprosi o banana dostaje pomarańcze lub gdy klient poprosi o
pomarańczę to dostaje banana a sprzedawca ściąga czapkę. W przypadku gdy sprzedawca
nie ma założonej czapki i klient poprosi o banana to go dostaje i czapka zostaje założona a
gdy poprosi o pomarańczę to dostaje pomarańczę a czapka zostaje nadal zdjęta. Na
serwerze jest tworzona lista zakupów danych klientów.

## Uruchomienie serwera:

W celu uruchomienia serwera wystarczy np z konsoli cmd uruchomić plik serwer.exe bez
żadnych parametrów.

## Uruchomienie klienta:

W celu uruchomienia klienta należy uruchomić plik klient.exe z parametrem o wartości
adresu serwera. Jeżeli jest to na tym samym komputerze pewnie będzie to localhost o
adresie 127.0.0..

## Posługiwanie się klientem

Jeżeli chcemy zamówić banana wystarczy, że wpiszemy w konsoli “banana” natomiast gdy
chcemy pomarańczę należy wpisać “orange”.Inne dane nie przejdą! W odpowiedzi od
serwera dostajemy to co sprzedawca nam da.


Przykład:

## Informacje na serwerze

Na serwerze mamy informację co klient zamówił , co się dzieje z czapką oraz mamy dostęp
do całościowej listy zakupów klienta. Klient jest wyrażany jako adres ip z unikalnym
identyfikatorem.
