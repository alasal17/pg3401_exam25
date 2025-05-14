#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "task3.h"

void show_menu() {
    printf("\n=== Flight Management Menu ===\n");
    printf("1. Add flight\n");
    printf("2. Add passenger to flight\n");
    printf("3. Print flight N\n");
    printf("4. Find flight by destination\n");
    printf("5. Delete flight\n");
    printf("6. Change passenger seat\n");
    printf("7. Search for passenger in all flights\n");
    printf("8. Find passengers on multiple flights\n");
    printf("9. Exit\n");
    printf("Choose option: ");
}

int main() {
    Flight* flightList = NULL;
    int option = 0;

    while (1) {
        show_menu();
        scanf("%d", &option);
        getchar();

        if (option == 9) break;

        char id[10], dest[50], name[50];
        int seats, time, seat, n;

        switch (option) {
            case 1:
                printf("Enter flight ID: ");
                fgets(id, sizeof(id), stdin); id[strcspn(id, "\n")] = 0;
                printf("Enter destination: ");
                fgets(dest, sizeof(dest), stdin); dest[strcspn(dest, "\n")] = 0;
                printf("Enter seat count: ");
                scanf("%d", &seats);
                printf("Enter departure time (e.g. 1430): ");
                scanf("%d", &time);
                getchar();
                flightList = add_flight(flightList, id, dest, seats, time);
                printf("Flight added.\n");
                break;

            case 2:
                printf("Enter flight ID to add passenger: ");
                fgets(id, sizeof(id), stdin); id[strcspn(id, "\n")] = 0;
                printf("Enter passenger name: ");
                fgets(name, sizeof(name), stdin); name[strcspn(name, "\n")] = 0;
                printf("Enter seat number: ");
                scanf("%d", &seat); getchar();
                {
                    Flight* f = flightList;
                    while (f) {
                        if (strcmp(f->flightId, id) == 0) {
                            add_passenger(f, name, seat);
                            printf("Passenger added.\n");
                            break;
                        }
                        f = f->next;
                    }
                }
                break;

            case 3:
                printf("Enter flight number (1 = first): ");
                scanf("%d", &n); getchar();
                {
                    Flight* f = flightList;
                    for (int i = 1; f && i < n; i++) f = f->next;
                    if (f) {
                        print_flight(f);
                    } else {
                        printf("Flight not found.\n");
                    }
                }
                break;

            case 4:
                printf("Enter destination to search for: ");
                fgets(dest, sizeof(dest), stdin); dest[strcspn(dest, "\n")] = 0;
                {
                    Flight* f = flightList;
                    int index = 1, found = 0;
                    while (f) {
                        if (strcmp(f->destination, dest) == 0) {
                            printf("Flight found at position %d: %s\n", index, f->flightId);
                            found = 1;
                            break;
                        }
                        index++;
                        f = f->next;
                    }
                    if (!found) printf("No flight found for destination %s\n", dest);
                }
                break;

            case 5:
                printf("Enter flight ID to delete: ");
                fgets(id, sizeof(id), stdin); id[strcspn(id, "\n")] = 0;
                flightList = delete_flight(flightList, id);
                break;

            case 6:
                printf("Enter flight ID: ");
                fgets(id, sizeof(id), stdin); id[strcspn(id, "\n")] = 0;
                printf("Enter passenger name: ");
                fgets(name, sizeof(name), stdin); name[strcspn(name, "\n")] = 0;
                printf("Enter new seat number: ");
                scanf("%d", &seat); getchar();
                change_passenger_seat(flightList, id, name, seat);
                break;

            case 7:
                printf("Enter passenger name to search: ");
                fgets(name, sizeof(name), stdin); name[strcspn(name, "\n")] = 0;
                search_passenger(flightList, name);
                break;

            case 8:
                find_duplicate_passengers(flightList);
                break;

            default:
                printf("Invalid option.\n");
        }
    }

    free_flights(flightList);
    return 0;
}
