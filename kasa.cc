#include <iostream>
#include <string>
#include <regex>
#include <sstream>
#include <limits>

using route = std::pair<int, std::vector<std::pair<int, std::string>>>;
using question = std::vector<std::pair<std::string, int>>;
using ticketName = std::vector<std::string>;
using ticketPrice = std::vector<int>;
using ticketTime = std::vector<int>;

// Funkcja sprawdza czy komenda nie zaczyna ani nie koĂączy sie spacja oraz
// czy slowa komendy sa oddzielone pojedyncza spacja.
bool isLineSeparatedBySingleSpaces(std::string line) {
    // korzystam z zalozenia, ze komenda nie zaczyna ani nie konczy sie spacja.
    if (line[0] == ' ' || line[line.size() - 1] == ' ')
        return false;

    for (size_t i = 0; i < line.size() - 1; i++) {
        if (line[i] == ' ' && line[i + 1] == ' ')
            return false;
    }

    return true;
}

// Funkcja dzieli string na wektor stringow.
void lineToWords(std::string str, std::vector<std::string>& line) {
    std::istringstream ss(str);

    std::string word = "";
    for (auto x : str) {
        if (x == ' ') {
            line.push_back(word);
            word = "";
        }
        else {
            word = word + x;
        }
    }
    line.push_back(word);
}

int convertToGrosz(std::string price) {
    bool type = false;
    std::string zlote;
    std::string grosze;
    for (size_t i = 0; i < price.size(); i++) {

        if (price[i] == '.')
            type = true;

        if (type && price[i] != '.') {
            grosze.push_back(price[i]);
        }
        else if (!type && price[i] != '.') {
            zlote.push_back(price[i]);
        }
    }

    if (type) {
        return 100 * stoi(zlote) + stoi(grosze);
    }
    else {
        return 100 * stoi(price);
    }
}

int convertToSec(std::string time) {
    bool type = false; // format:  23
    std::string hours;
    std::string minutes;
    for (size_t i = 0; i < time.size(); i++) {
        if (time[i] == ':') {
            type = true; // format:  23:34

        }
        if (type == true && time[i] != ':') {
            minutes.push_back(time[i]);
        }
        else if (type == false && time[i] != ':') {
            hours.push_back(time[i]);
        }
    }

    if (type) {
        return 3600 * stoi(hours) + 60 * stoi(minutes);
    }
    else {
        return 60 * stoi(time);
    }
}

// Funkcja sprawdza czy godzina miesci sie w wyspecyfikowanym zakresie.
bool isValidHour(std::string hour) {
    size_t i = 0;
    while (hour[i] != ':')
        i++;
    i++;
    if (hour[i] > 53)
        return false;

    int stopTime = convertToSec(hour);
    int start = convertToSec("5:55");
    int end = convertToSec("21:21");

    if (stopTime < start || stopTime > end)
        return false;
    return true;
}


// Przypisuje na zmienna r nowy kurs, argument str jest pojedynczym wierszem
// wejscia.
bool addRoute(route& r, std::string str) {
    std::vector<std::string> line;
    lineToWords(str, line);

    int routeNum = stoi(line[0]);

    if (routeNum < 0)
        return false;

    std::vector<std::pair<int, std::string>> timetable;
    size_t i = 1;
    int previous = convertToSec(line[i]);

    while (i < line.size()) {
        std::pair<int, std::string> newPair;
        newPair.first = convertToSec(line[i]);
        i++;
        newPair.second = line[i];
        timetable.push_back(newPair);

        i++;
        if (i < line.size() && convertToSec(line[i]) <= previous)
            return false;

        if (i < line.size())
            previous = convertToSec(line[i]);
    }

    r.first = routeNum;
    r.second = timetable;
    return true;
}

// Dodaje bilet do bazy biletow. Nazwa, cena i czas waznosci sa trzymane
// w oddzielnych wektorach. Wartosci z indeksu o numerze i charakteryzuja
// jeden bilet.
void addTicket(ticketName& tn, ticketPrice& tp, ticketTime& tt,
    std::vector<std::string> line) {
    tn.push_back(line[0]);
    tp.push_back(convertToGrosz(line[1]));
    tt.push_back(convertToSec(line[2]));
}

inline bool isNumber(const std::string& s) {
    if (s.empty() || ((!isdigit(s[0])) && (s[0] != '-') && (s[0] != '+')))
        return false;

    char* p;
    strtol(s.c_str(), &p, 10);

    return (*p == 0);
}

// Sprawdza czy godzina jest zapisana w poprawnym formacie.
bool isProperTime(std::string time) {
    if (time.size() != 5 && time.size() != 4)
        return false;
    if (time.size() == 5 && time[2] != ':')
        return false;
    if (time.size() == 4 && time[1] != ':')
        return false;
    if (!isdigit(time[0]) || time[0] == '0')
        return false;
    if (time.size() == 5 && (!isdigit(time[1]) || !isdigit(time[3]) ||
        !isdigit(time[4])))
        return false;
    if (time.size() == 4 && (!isdigit(time[2]) || !isdigit(time[3])))
        return false;
    if (!isValidHour(time))
        return false;

    return true;
}

// Sprawdza czy nazwa przystanku spelnia specyfikacje zadania.
bool isProperStopName(std::string  name) {
    if (name.size() == 0)
        return false;

    for (size_t i = 0; i < name.size(); i++) {
        if (!(isalpha(name[i]) || name[i] == '_' || name[i] == '^'))
            return false;
    }

    return true;
}

// Sprawdza poprawnosc komendy dodania rozkladu. funkcja nie sprawdza czy
// godziny sa rosnace, tym zajmuje sie funkcja addRoute.
bool isValidRoute(std::string str, std::vector<route> routes) {
    if (!isLineSeparatedBySingleSpaces(str))
        return false;

    std::vector<std::string> line;
    lineToWords(str, line);

    if (line.size() < 3)
        return false;
    if (!isNumber(line[0]))
        return false;

    size_t i = 1;
    while (i < line.size()) {
        if (!isProperTime(line[i])) {
            return false;
        }
        i++;
        if (i >= line.size()) {
            return false;
        }
        if (!isProperStopName(line[i]))
            return false;

        i++;
    }

    std::vector<std::string> stops;
    for (size_t i = 2; i < line.size(); i++) {
        if (i >= 2 && i % 2 == 0)
            stops.push_back(line[i]);
    }

    for (size_t i = 0; i < stops.size(); i++) {
        for (size_t j = i; j < stops.size(); j++) {
            if (i != j) {
                if (stops[i] == stops[j])
                    return false;
            }
        }
    }

    for (size_t i = 0; i < routes.size(); i++) {
        if (routes[i].first == stoi(line[0]))
            return false;
    }

    return true;
}


// Nazwa biletu ma inne specyfikacja niz nazwa przystanku, stad
// odrebna funkcja na jej sprawdzanie.
bool isProperTicketName(std::string name) {
    if (name.size() == 0)
        return false;

    for (size_t i = 0; i < name.size(); i++) {
        if (!(isalpha(name[i]) || name[i] == ' '))
            return false;
    }

    return true;
}

// Sprawdza czy cena jest w poprawnym formacie.
bool isProperPrice(std::string price) {
    size_t i = 0;
    while (i < price.size() && price[i] != '.') {
        if (!isdigit(price[i]))
            return false;
        i++;
    }

    if (i >= price.size())
        return false;


    i++;
    if (i >= price.size())
        return false;
    int count = 0;
    while (i < price.size()) {
        if (!isdigit(price[i]))
            return false;
        count++;
        i++;
    }

    if (!(count == 2))
        return false;
        
    try {
        count = stoi(price);
    }
    catch(...) {
        return false;
    }

    return true;

}

// Sprawdza czy czas waznosci biletu nie ma zer wiodacych.
bool withoutTrailingZeros(std::string str) {
    if (str[0] == '0')
        return false;
    else
        return true;
}

// Wyodrebnia prefiks stringa str do indeksu divisionIndex wlacznie.
void prefix(std::string str, std::string& pref, size_t divisionIndex) {
    size_t i = 0;

    while (i <= divisionIndex) {
        pref.push_back(str[i]);
        i++;
    }
}

// Dziala podobnie do funkcji sufix, wyodrebnia jednak sufiks stringa.
// Funkcje prefix i sufix uzywane sa przy sprawdzaniu poprawnosci
// komendy dodania biletu. Jest to spowodowane zezwoleniem na
// wystepowanie spacji w nazwie biletu.
void sufix(std::string str, std::string& suf, size_t divisionIndex) {
    size_t i = divisionIndex + 1;

    while (i < str.size()) {
        suf.push_back(str[i]);
        i++;
    }
}

// Sprawdza poprawnosc komendy dodania biletu. Wiersz jest dzielony na dwie
// czesci. Pierwsza zawiera potencjalna nazwe a druga reszte wiersza.
// Funkcja dziala w ten sposob gdyz w tym przypadku nie dziala dzielenie
// wiersza na pojedyncze slowa poniewaz w nazwie wystepuja legalnie spacje.
bool isValidTicket(std::string ticket, ticketName names,
    std::vector<std::string>& line) {
    std::vector<std::string> temporary;
    size_t i = 0;
    if (ticket.size() < 5)
        return false;
    if (isdigit(ticket[0]) || isdigit(ticket[1]))
        return false;

    while (i < ticket.size() && !isdigit(ticket[i])) {
        if (ticket[i] != ' ' && !isalpha(ticket[i]))
            return false;

        i++;
    }

    if (i >= ticket.size())
        return false;
    if (ticket[i - 1] != ' ')
        return false;

    std::string name;
    prefix(ticket, name, i - 2);
    line.push_back(name);
    std::string timeAndPrice;
    sufix(ticket, timeAndPrice, i - 1);

    if (!isLineSeparatedBySingleSpaces(timeAndPrice))
        return false;
    lineToWords(timeAndPrice, temporary);
    if (temporary.size() != 2)
        return false;

    line.push_back(temporary[0]);
    line.push_back(temporary[1]);

    if (!isProperPrice(line[1]))
        return false;
    if (!isNumber(line[2]))
        return false;
    if (!withoutTrailingZeros(line[2]))
        return false;

    for (i = 0; i < names.size(); i++) {
        if (line[0] == names[i])
            return false;
    }
    return true;
}

// Sprawdza czy odpowiedni kurs odwiedza sasiadujace z nim (z numerem kursu
// w wierszu wejscia) w wierszu stacje.
bool checkIfRouteVisitsStops(std::vector<route> routes, int indexOfRouteNumber,
    std::vector<std::string> line) {
    size_t i = 0;

    while (i < routes.size() && routes[i].first != stoi(line[indexOfRouteNumber])) {
        i++;
    }

    route r = routes[i];
    bool isPrecedingStop = false;
    bool isFollowingStop = false;
    int indexOfPreceding = 0;
    int indexOfFollowing = 0;
    for (size_t j = 0; j < r.second.size(); j++) {
        if (r.second[j].second == line[indexOfRouteNumber - 1]) {
            isPrecedingStop = true;
            indexOfPreceding = j;
        }

        if (r.second[j].second == line[indexOfRouteNumber + 1]) {
            isFollowingStop = true;
            indexOfFollowing = j;
        }
    }

    if (isPrecedingStop && isFollowingStop) {
        if (r.second[indexOfPreceding].first < r.second[indexOfFollowing].first)
            return true;
        else
            return false;
    }
    return false;
}

// Ma sprawdzic czy istnieje polaczenie dla zapytania ale nie sprawdza jeszcze
// czy pasazer zdazy sie przesiasc.
bool existsConnection(std::vector<route> routes,
    std::vector<std::string> line) {
    for (size_t i = 2; i < line.size(); i++) {
        if (i % 2 == 0) {
            if (!checkIfRouteVisitsStops(routes, i, line))
                return false;
        }
    }

    return true;
}

// Szuka trasy o danym numerze w zbiorze dodanych rozkladow.
int findRoute(std::vector<route> routes, int numberOfRoute) {
    size_t i = 0;
    while (i < routes.size() && routes[i].first != numberOfRoute) {
        i++;
    }

    return i;
}

// Szuka danego przystanku w danej trasie, zwraca jego indeks.
int findStop(std::string stop, route r) {
    size_t i = 0;
    while (i < r.second.size() && r.second[i].second != stop) {
        i++;
    }

    return i;
}

// Sprawdza dla danego zapytania czy pasazer zdazy sie przesiadac do
// kolejnych tramwajow (czy nie odjada zanim dojedzie do przystanku),
// zakladajac, ze polaczenie istnieje.
bool isItPossibleToChangeRout(std::vector<route> routes,
    std::vector<std::string> line, int currentIndex) {
    int index = findRoute(routes, stoi(line[currentIndex]));
    route r = routes[index];
    int i = findStop(line[currentIndex + 1], r);
    int firstTime = r.second[i].first;
    index = findRoute(routes, stoi(line[currentIndex + 2]));
    r = routes[index];
    i = findStop(line[currentIndex + 1], r);
    int secondTime = r.second[i].first;

    return (firstTime <= secondTime);
}

// Stanowi otoczke dla funkcji powyzej.
bool hoursInProperOrder(std::vector<route> routes,
    std::vector<std::string> line) {
    for (size_t i = 2; i < line.size() - 2; i += 2) {
        if (i < line.size()) {
            if (!isItPossibleToChangeRout(routes, line, i))
                return false;
        }
    }

    return true;
}

// Sprawdza poprawnosc zapytania.
bool isValidQuestion(std::string question, std::vector<route> routes) {
    if (!isLineSeparatedBySingleSpaces(question))
        return false;

    std::vector<std::string> line;
    lineToWords(question, line);

    if (line.size() < 4)
        return false;
    if (line[0][0] != '?')
        return false;
    if (!isProperStopName(line[1]))
        return false;

    size_t i = 2;
    while (i < line.size()) {
        if (i % 2 == 0 && !isNumber(line[i])) {
            return false;
        }
        if (i % 2 == 1 && !isProperStopName(line[i])) {
            return false;
        }
        i++;
    }

    if (i % 2 == 1)
        return false;

    bool result;
    for (i = 2; i < line.size(); i++) {
        result = false;
        if (i % 2 == 0) {
            for (size_t j = 0; j < routes.size(); j++) {
                if (stoi(line[i]) == routes[j].first)
                    result = true;
            }
            if (!result)
                return false;
        }
    }

    if (!existsConnection(routes, line))
        return false;

    if (!hoursInProperOrder(routes, line))
        return false;

    return true;
}

void error(int line, std::string str) {
    std::cerr << "Error in line " << line << ":" << " " << str << std::endl;
}

//Zamienia string z wejscia na question.
question makeQuestion(std::string str) {
    question q;
    std::pair<std::string, int> p;
    std::vector<std::string> line;
    lineToWords(str, line);


    for (size_t i = 0; i < ((line.size() - 1) / 2); i++) {
        int it = (i * 2) + 1;
        p = std::make_pair(line[it], stoi(line[it + 1]));
        q.push_back(p);
    }

    p = std::make_pair(line[line.size() - 1], -1);
    q.push_back(p);
    return q;
}

//Zwraca czas trasy z pytania.
//Zwraca -1 jesli na ktoryms przystanku trzeba czekac.
int routeTime(question q, std::vector<route> routes) {
    int it, it2, time1, time2 = 0, result = 0;
    for (size_t i = 0; i < (q.size() - 1); i++) {
        it = 0;
        it2 = 0;
        //szukam trasy o id takim jak ta w pytaniu
        while (routes[it].first != q[i].second)
            it++;
        //w tej trasie szukam przystanku takiego jak w pytaniu
        while (routes[it].second[it2].second != q[i].first)
            it2++;
        time1 = routes[it].second[it2].first;

        if (i > 0 && time1 != time2) {
            std::cout << ":-( " << routes[it].second[it2].second << std::endl;
            return -1;
        }

        while (routes[it].second[it2].second != q[i + 1].first)
            it2++;
        time2 = routes[it].second[it2].first;
        result += (time2 - time1);
    }
    return result;
}


//Znajduje i wypisuje najtanszy zestaw biletow na okreslony czas.
//Zwraca ilosc uzytych biletow.
int findTickets(int time, ticketName names, ticketPrice prices,
    ticketTime times) {
    std::string ticket1, ticket2, ticket3;
    int bestPrice = std::numeric_limits<int>::max();
    int price, currentTime;

    //dodanie sztucznego biletu ktory jest darmowy i nie trwa ani sekukndy
    names.push_back("1");
    prices.push_back(0);
    times.push_back(0);

    size_t n = prices.size();

    for (size_t i = 0; i < n; i++) {
        for (size_t j = 0; j < n; j++) {
            for (size_t k = 0; k < n; k++) {
                price = prices[i] + prices[j] + prices[k];
                currentTime = times[i] + times[j] + times[k];
                if (price < bestPrice && currentTime > time) {
                    bestPrice = price;
                    ticket1 = names[i];
                    ticket2 = names[j];
                    ticket3 = names[k];
                }
            }
        }
    }
    if (bestPrice == std::numeric_limits<int>::max()) {
        std::cout << ":-|" << std::endl;
        return 0;
    }
    else {
        int x = 0;

        if (ticket3 != "1" && ticket2 != "1")
            ticket2 += "; ";
        if (ticket1 != "1" && (ticket2 != "1" || ticket3 != "1"))
            ticket1 += "; ";

        std::cout << "! ";
        if (ticket1 != "1") {
            std::cout << ticket1;
            x++;
        }
        if (ticket2 != "1") {
            std::cout << ticket2;
            x++;
        }
        if (ticket3 != "1") {
            std::cout << ticket3;
            x++;
        }
        std::cout << std::endl;

        return x;
    }
}

int main() {
    ticketName names;
    ticketPrice prices;
    ticketTime times;
    std::vector<route> routes;
    std::string str;
    int line = 1;
    int ticketsUsed = 0;

    while (getline(std::cin, str)) {
        std::vector<std::string> ticket;
        if (str.empty()) {
            //nie rob nic
        }
        else if (isValidRoute(str, routes)) {
            route r;
            bool orderedHours = addRoute(r, str);

            if (!orderedHours)
                error(line, str);
            if (orderedHours)
                routes.push_back(r);
        }
        else if (isValidQuestion(str, routes)) {
            question q = makeQuestion(str);
            int time = routeTime(q, routes);
            if (time > -1) {
                ticketsUsed += findTickets(time, names, prices, times);
            }
        }
        else if (isValidTicket(str, names, ticket)) {
            addTicket(names, prices, times, ticket);
        }
        else {
            error(line, str);
        }
        line++;
    }
    std::cout << ticketsUsed << std::endl;
}
