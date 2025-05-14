#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "task3.h"

// Function to add a new flight to the beginning of the double linked list
Flight* add_flight(Flight* head, const char* id, const char* destination, int seats, int departureTime) {
    Flight* newFlight = (Flight*)malloc(sizeof(Flight));
    if (!newFlight) {
        fprintf(stderr, "Memory allocation failed for new flight.\n");
        return head;
    }

    strncpy(newFlight->flightId, id, MAX_ID_LEN - 1);
    newFlight->flightId[MAX_ID_LEN - 1] = '\0';
    strncpy(newFlight->destination, destination, MAX_DEST_LEN - 1);
    newFlight->destination[MAX_DEST_LEN - 1] = '\0';
    newFlight->seatCount = seats;
    newFlight->departureTime = departureTime;
    newFlight->passengers = NULL;
    newFlight->prev = NULL;
    newFlight->next = head;

    if (head != NULL) {
        head->prev = newFlight;
    }

    return newFlight;
}

// Add a passenger sorted by seat number
Passenger* add_passenger(Flight* flight, const char* name, int seatNumber) {
    if (!flight || seatNumber < 1 || seatNumber > flight->seatCount) {
        fprintf(stderr, "Invalid flight or seat number.\n");
        return NULL;
    }

    Passenger* newPassenger = (Passenger*)malloc(sizeof(Passenger));
    if (!newPassenger) {
        fprintf(stderr, "Memory allocation failed for passenger.\n");
        return NULL;
    }

    strncpy(newPassenger->name, name, MAX_NAME_LEN - 1);
    newPassenger->name[MAX_NAME_LEN - 1] = '\0';
    newPassenger->seatNumber = seatNumber;
    newPassenger->age = 0; // default age
    newPassenger->next = NULL;

    // Sorted insertion
    Passenger** pp = &flight->passengers;
    while (*pp && (*pp)->seatNumber < seatNumber) {
        pp = &(*pp)->next;
    }
    newPassenger->next = *pp;
    *pp = newPassenger;

    return newPassenger;
}

// Print all flights and passengers
void print_flights(Flight* list) {
    while (list != NULL) {
        print_flight(list);
        list = list->next;
        printf("\n");
    }
}

// Print a single flight
void print_flight(Flight* flight) {
    if (!flight) {
        printf("Flight is NULL.\n");
        return;
    }

    printf("Flight ID: %s\n", flight->flightId);
    printf("Destination: %s\n", flight->destination);
    printf("Seats: %d\n", flight->seatCount);
    printf("Departure Time: %d\n", flight->departureTime);

    printf("Passengers:\n");
    Passenger* p = flight->passengers;
    while (p != NULL) {
        printf("  Name: %s | Seat: %d | Age: %d\n", p->name, p->seatNumber, p->age);
        p = p->next;
    }
}

// Remove a passenger
void remove_passenger(Flight* list, const char* flightId, const char* passengerName) {
    while (list != NULL) {
        if (strcmp(list->flightId, flightId) == 0) {
            Passenger* current = list->passengers;
            Passenger* previous = NULL;

            while (current != NULL) {
                if (strcmp(current->name, passengerName) == 0) {
                    if (previous == NULL) {
                        list->passengers = current->next;
                    } else {
                        previous->next = current->next;
                    }
                    free(current);
                    printf("Passenger %s removed from flight %s.\n", passengerName, flightId);
                    return;
                }
                previous = current;
                current = current->next;
            }

            printf("Passenger %s not found on flight %s.\n", passengerName, flightId);
            return;
        }
        list = list->next;
    }

    printf("Flight %s not found.\n", flightId);
}

// Free the entire flight list and all passengers
void free_flights(Flight* list) {
    while (list != NULL) {
        Passenger* p = list->passengers;
        while (p != NULL) {
            Passenger* tmp = p;
            p = p->next;
            free(tmp);
        }

        Flight* tmpFlight = list;
        list = list->next;
        free(tmpFlight);
    }
}

// Delete a flight from the double linked list
Flight* delete_flight(Flight* head, const char* flightId) {
    Flight* current = head;

    while (current != NULL) {
        if (strcmp(current->flightId, flightId) == 0) {
            // Delete a flight from the double linked list
            if (current->prev) {
                current->prev->next = current->next;
            } else {
                head = current->next;
            }

            if (current->next) {
                current->next->prev = current->prev;
            }

            Passenger* p = current->passengers;
            while (p != NULL) {
                Passenger* tmp = p;
                p = p->next;
                free(tmp);
            }

            free(current);
            printf("Flight %s deleted.\n", flightId);
            return head;
        }

        current = current->next;
    }

    printf("Flight %s not found.\n", flightId);
    return head;
}

// Change a passenger’s seat
void change_passenger_seat(Flight* list, const char* flightId, const char* passengerName, int newSeatNumber) {
    while (list) {
        if (strcmp(list->flightId, flightId) == 0) {
            Passenger* current = list->passengers;
            Passenger* prev = NULL;

            // Find passenger
            while (current) {
                if (strcmp(current->name, passengerName) == 0) {
                    // Remove and re-insert
                    if (prev) prev->next = current->next;
                    else list->passengers = current->next;

                    int age = current->age;
                    free(current);
                    Passenger* pNew = add_passenger(list, passengerName, newSeatNumber);
                    if (pNew) pNew->age = age;
                    printf("Seat changed for %s to %d.\n", passengerName, newSeatNumber);
                    return;
                }
                prev = current;
                current = current->next;
            }

            printf("Passenger %s not found.\n", passengerName);
            return;
        }
        list = list->next;
    }

    printf("Flight %s not found.\n", flightId);
}

// Search for a passenger across all flights
void search_passenger(Flight* list, const char* passengerName) {
    int found = 0;
    while (list) {
        Passenger* p = list->passengers;
        while (p) {
            if (strcmp(p->name, passengerName) == 0) {
                printf("Found %s on flight %s (seat %d)\n", passengerName, list->flightId, p->seatNumber);
                found = 1;
            }
            p = p->next;
        }
        list = list->next;
    }

    if (!found) {
        printf("Passenger %s not found on any flight.\n", passengerName);
    }
}

// Find passengers booked on more than one flight
void find_duplicate_passengers(Flight* list) {
    Flight* f1 = list;

    while (f1) {
        Passenger* p1 = f1->passengers;
        while (p1) {
            int count = 0;
            Flight* f2 = list;
            while (f2) {
                Passenger* p2 = f2->passengers;
                while (p2) {
                    if (strcmp(p1->name, p2->name) == 0) {
                        count++;
                        break;
                    }
                    p2 = p2->next;
                }
                f2 = f2->next;
            }

            if (count > 1) {
                printf("Passenger %s is booked on multiple flights.\n", p1->name);
            }

            p1 = p1->next;
        }

        f1 = f1->next;
    }
}
