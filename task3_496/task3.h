#ifndef TASK3_H
#define TASK3_H

#define MAX_NAME_LEN 50
#define MAX_ID_LEN 10
#define MAX_DEST_LEN 50

// Passenger (single linked list)
typedef struct Passenger {
    int seatNumber;
    char name[MAX_NAME_LEN];
    int age;
    struct Passenger* next;
} Passenger;

// Flight (double linked list)
typedef struct Flight {
    char flightId[MAX_ID_LEN];
    char destination[MAX_DEST_LEN];
    int seatCount;
    int departureTime;
    Passenger* passengers;
    struct Flight* prev;
    struct Flight* next;
} Flight;


// Add a flight to the list
Flight* add_flight(Flight* head, const char* id, const char* destination, int seats, int departureTime);

// Add a passenger to a flight (Note: must be sorted by seatNumber)
Passenger* add_passenger(Flight* flight, const char* name, int seatNumber);

// Print all flights and passengers (for testing)
void print_flights(Flight* list);

// Print a specific flight
void print_flight(Flight* flight);

// Remove a passenger (used for testing)
void remove_passenger(Flight* list, const char* flightId, const char* passengerName);

// Free the entire flight list along with all passengers
void free_flights(Flight* list);

// Delete a flight and all its passengers
Flight* delete_flight(Flight* head, const char* flightId);

// Change seat for a passenger in a given flight
void change_passenger_seat(Flight* list, const char* flightId, const char* passengerName, int newSeatNumber);

// Search for a passenger across all flights
void search_passenger(Flight* list, const char* passengerName);

// Find passengers booked on more than one flight
void find_duplicate_passengers(Flight* list);

#endif
