#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>

#define MAX_BUSES 10
#define MAX_BOOKINGS 50
#define MAX_USERS 20

// ======================================
// DATA STRUCTURES
// ======================================
struct Bus {
    int bus_id;
    char from_city[30];
    char to_city[30];
    char dep_time[10];
    char arr_time[10];
    char driver[30];
    int driver_rating;
    int route_safety;
    char current_route[50];
    int total_seats;
    int available_seats;
};

struct Booking {
    int booking_id;
    int bus_id;
    char phone[15];
    char email[50];
    char passenger_name[30];
    int seat_number;
    char booking_time[20];
    int status; // 1=confirmed, 0=cancelled
};

struct User {
    char phone[15];
    char email[50];
    char name[30];
};

// ======================================
// GLOBAL DATA
// ======================================
struct Bus buses[MAX_BUSES];
struct Booking bookings[MAX_BOOKINGS];
struct User users[MAX_USERS];
int booking_count = 0;
int user_count = 0;

// ======================================
// UTILITY FUNCTIONS
// ======================================
void toLowerCase(char* str) {
    for (int i = 0; str[i]; i++) {
        str[i] = tolower(str[i]);
    }
}

int strcasecmp_custom(const char* s1, const char* s2) {
    char temp1[100], temp2[100];
    strncpy(temp1, s1, sizeof(temp1)-1);
    strncpy(temp2, s2, sizeof(temp2)-1);
    temp1[sizeof(temp1)-1] = '\0';
    temp2[sizeof(temp2)-1] = '\0';
    toLowerCase(temp1);
    toLowerCase(temp2);
    return strcmp(temp1, temp2);
}

int getHourFromTime(const char* time_str) {
    int hour;
    sscanf(time_str, "%d", &hour);
    return hour;
}

int isNightTravel(const char* dep_time, const char* arr_time) {
    int dep_hour = getHourFromTime(dep_time);
    int arr_hour = getHourFromTime(arr_time);
    return (dep_hour >= 20 || arr_hour >= 20);
}

int calculateAdjustedSafety(const struct Bus* b) {
    int base_safety = b->driver_rating + b->route_safety;
    return isNightTravel(b->dep_time, b->arr_time) ? base_safety - 2 : base_safety;
}

// ======================================
// CORE BUSINESS LOGIC
// ======================================
void sendTicketConfirmation(const struct Booking* booking, const struct Bus* bus) {
    printf("\n📱 SMS SENT TO: %s\n", booking->phone);
    printf("📧 EMAIL SENT TO: %s\n", booking->email);
    printf("========================================\n");
    printf("🎫 TICKET CONFIRMED!\n");
    printf("Booking ID: %d\n", booking->booking_id);
    printf("Bus ID: %d (%s → %s)\n", bus->bus_id, bus->from_city, bus->to_city);
    printf("Passenger: %s\n", booking->passenger_name);
    printf("Seat: %d\n", booking->seat_number);
    printf("Departure: %s\n", bus->dep_time);
    printf("Status: CONFIRMED ✅\n");
    printf("========================================\n");
    printf("Thank you for booking with SmartBus!\n\n");
}

int registerUser(const char* phone, const char* email, const char* name) {
    for (int i = 0; i < user_count; i++) {
        if (strcmp(users[i].phone, phone) == 0 || strcmp(users[i].email, email) == 0) {
            printf("👤 User already registered!\n");
            return i;
        }
    }
    
    strncpy(users[user_count].phone, phone, sizeof(users[user_count].phone)-1);
    strncpy(users[user_count].email, email, sizeof(users[user_count].email)-1);
    strncpy(users[user_count].name, name, sizeof(users[user_count].name)-1);
    users[user_count].phone[sizeof(users[user_count].phone)-1] = '\0';
    users[user_count].email[sizeof(users[user_count].email)-1] = '\0';
    users[user_count].name[sizeof(users[user_count].name)-1] = '\0';
    
    printf("✅ User registered successfully!\n");
    return user_count++;
}

void bookBus(int bus_index, const char* phone, const char* email, const char* passenger_name) {
    if (buses[bus_index].available_seats <= 0) {
        printf("❌ No seats available!\n");
        return;
    }
    
    int user_index = registerUser(phone, email, passenger_name);
    
    struct Booking new_booking = {0};
    new_booking.booking_id = 1000 + booking_count;
    new_booking.bus_id = buses[bus_index].bus_id;
    strncpy(new_booking.phone, phone, sizeof(new_booking.phone)-1);
    strncpy(new_booking.email, email, sizeof(new_booking.email)-1);
    strncpy(new_booking.passenger_name, passenger_name, sizeof(new_booking.passenger_name)-1);
    new_booking.seat_number = buses[bus_index].total_seats - buses[bus_index].available_seats + 1;
    
    time_t now = time(NULL);
    strftime(new_booking.booking_time, sizeof(new_booking.booking_time), "%Y-%m-%d %H:%M:%S", localtime(&now));
    new_booking.status = 1;
    
    bookings[booking_count] = new_booking;
    buses[bus_index].available_seats--;
    
    sendTicketConfirmation(&new_booking, &buses[bus_index]);
    booking_count++;
}

void cancelBooking(int booking_id) {
    for (int i = 0; i < booking_count; i++) {
        if (bookings[i].booking_id == booking_id && bookings[i].status == 1) {
            for (int j = 0; j < MAX_BUSES; j++) {
                if (buses[j].bus_id == bookings[i].bus_id) {
                    buses[j].available_seats++;
                    break;
                }
            }
            bookings[i].status = 0;
            printf("✅ Booking %d cancelled successfully.\n", booking_id);
            printf("💰 Refund processed to %s\n\n", bookings[i].phone);
            return;
        }
    }
    printf("❌ Booking ID not found or already cancelled!\n\n");
}

// ======================================
// DATA INITIALIZATION
// ======================================
void initBuses() {
    struct Bus bus_data[MAX_BUSES] = {
        {101, "Pune", "Mumbai", "06:30", "10:30", "Raj", 4, 4, "Pune-NH48-Khandala", 40, 35},
        {102, "Pune", "Mumbai", "09:00", "13:30", "Anil", 3, 2, "Pune-NH48-Lonavala", 40, 38},
        {103, "Pune", "Nashik", "07:00", "11:00", "Vijay", 5, 4, "Pune-Sangamner-Nashik", 35, 30},
        {104, "Mumbai", "Pune", "21:00", "01:00", "Suresh", 4, 4, "Mumbai-NH48-Panvel", 40, 32},
        {105, "Pune", "Kolhapur", "08:00", "12:00", "Kiran", 3, 3, "Pune-Satara-Kolhapur", 45, 40},
        {106, "Nashik", "Pune", "22:00", "02:00", "Ramesh", 4, 3, "Nashik-Sangamner-Pune", 35, 28},
        {107, "Pune", "Mumbai", "14:00", "18:00", "Mahesh", 4, 4, "Pune-NH48-Khopoli", 40, 36},
        {108, "Pune", "Nashik", "15:00", "19:00", "Sunil", 3, 3, "Pune-Shirdi-Nashik", 35, 31},
        {109, "Mumbai", "Nashik", "10:00", "15:00", "Ajay", 2, 2, "Mumbai-NashikRd-Igatpuri", 40, 25},
        {110, "Kolhapur", "Pune", "16:00", "20:00", "Prakash", 4, 3, "Kolhapur-Satara-Pune", 45, 42}
    };
    
    for (int i = 0; i < MAX_BUSES; i++) {
        buses[i] = bus_data[i];
    }
}

int cityExists(const char* city) {
    for (int i = 0; i < MAX_BUSES; i++) {
        if (strcasecmp_custom(buses[i].from_city, city) == 0 || 
            strcasecmp_custom(buses[i].to_city, city) == 0) {
            return 1;
        }
    }
    return 0;
}

void emergencySOS(int bus_id, const char* current_location) {
    printf("\n 🚨 EMERGENCY SOS ACTIVATED! 🚨 \n");
    printf("========================================\n");
    printf(" 🔴 ALERT SENT TO EMERGENCY CONTACTS\n");
    printf(" 🚌 Bus ID: %d\n", bus_id);
    printf(" 📍 Current Location: %s\n", current_location);
    
    time_t now = time(NULL);
    char time_str[20];
    strftime(time_str, sizeof(time_str), "%H:%M:%S", localtime(&now));
    printf(" ⏰ Time: %s IST\n", time_str);
    printf("========================================\n");
    printf(" ✅ SOS Message Sent Successfully!\n");
    printf(" Help is on the way! Stay safe.\n\n");
}

// ======================================
// MAIN PROGRAM
// ======================================
int main() {
    initBuses();
    int choice, selected_bus;
    
    printf("=== 🚌 SMART BUS FINDER & BOOKING SYSTEM ===\n\n");
    
    while (1) {
        printf("1. 🔍 Find bus BETWEEN cities\n");
        printf("2. 📋 Show ALL routes FROM city\n");
        printf("3. 🎫 BOOK Ticket\n");
        printf("4. ❌ Cancel Booking\n");
        printf("5. 🚨 EMERGENCY SOS\n");
        printf("6. 🚪 Exit\n");
        printf("Enter choice: ");
        
        if (scanf("%d", &choice) != 1) {
            while (getchar() != '\n');
            printf("❌ Invalid input! Please enter a number.\n\n");
            continue;
        }
        
        if (choice == 6) break;
        
        if (choice == 5) {
            printf("Enter your Bus ID: ");
            scanf("%d", &selected_bus);
            for (int i = 0; i < MAX_BUSES; i++) {
                if (buses[i].bus_id == selected_bus) {
                    emergencySOS(buses[i].bus_id, buses[i].current_route);
                    break;
                }
            }
            continue;
        }
        
        if (choice < 1 || choice > 4) {
            printf("❌ Invalid choice!\n\n");
            continue;
        }
        
        char source[30], destination[30];
        printf("Enter source city: ");
        scanf("%29s", source);
        
        if (!cityExists(source)) {
            printf("❌ No such city '%s' in dataset!\n\n", source);
            continue;
        }
        
        if (choice == 1 || choice == 3) {
            printf("Enter destination city: ");
            scanf("%29s", destination);
            if (!cityExists(destination)) {
                printf("❌ No such city '%s' in dataset!\n\n", destination);
                continue;
            }
        }
        
        // Option 1: Find buses between cities
        if (choice == 1) {
            printf("\n%s → %s:\n", source, destination);
            printf("ID  Seats Driver  Time    Safety\n");
            printf("-----------------------------------\n");
            
            int found_count = 0;
            for (int i = 0; i < MAX_BUSES; i++) {
                if (strcasecmp_custom(buses[i].from_city, source) == 0 && 
                    strcasecmp_custom(buses[i].to_city, destination) == 0) {
                    int safety = calculateAdjustedSafety(&buses[i]);
                    int booked = buses[i].total_seats - buses[i].available_seats;
                    printf("%-3d %2d/%-2d %-8s %-8s %2d/10\n",
                           buses[i].bus_id, booked, buses[i].total_seats, 
                           buses[i].driver, buses[i].dep_time, safety);
                    found_count++;
                }
            }
            if (!found_count) printf("\n❌ No direct buses found.\n");
            printf("\n");
            
        // Option 2: Show all routes from source city
        } else if (choice == 2) {
            printf("\nAll routes FROM %s:\n", source);
            printf("ID  To      Time  Seats  Driver\n");
            printf("--------------------------------\n");
            
            int found_count = 0;
            for (int i = 0; i < MAX_BUSES; i++) {
                if (strcasecmp_custom(buses[i].from_city, source) == 0) {
                    int booked = buses[i].total_seats - buses[i].available_seats;
                    printf("%-3d %-8s %-6s %2d/%-2d %s\n",
                           buses[i].bus_id, buses[i].to_city, buses[i].dep_time,
                           booked, buses[i].total_seats, buses[i].driver);
                    found_count++;
                }
            }
            if (!found_count) printf("❌ No routes found from %s\n", source);
            printf("\n");
            
        // Option 3: Book ticket
        } else if (choice == 3) {
            printf("\nAvailable buses %s → %s:\n", source, destination);
            int valid_buses[MAX_BUSES], valid_count = 0;
            
            for (int i = 0; i < MAX_BUSES; i++) {
                if (strcasecmp_custom(buses[i].from_city, source) == 0 && 
                    strcasecmp_custom(buses[i].to_city, destination) == 0 &&
                    buses[i].available_seats > 0) {
                    printf("%d. Bus %d - %s (%d/%d seats left)\n", 
                           valid_count+1, buses[i].bus_id, buses[i].driver, 
                           buses[i].available_seats, buses[i].total_seats);
                    valid_buses[valid_count++] = i;
                }
            }
            
            if (valid_count == 0) {
                printf("❌ No available buses for this route!\n\n");
                continue;
            }
            
            printf("Enter bus number (1-%d): ", valid_count);
            int bus_num;
            scanf("%d", &bus_num);
            bus_num--;
            
            if (bus_num >= 0 && bus_num < valid_count) {
                int bus_index = valid_buses[bus_num];
                char phone[15], email[50], name[30];
                
                printf("Enter phone number: ");
                scanf("%14s", phone);
                printf("Enter email: ");
                scanf("%49s", email);
                printf("Enter passenger name: ");
                scanf("%29s", name);
                
                bookBus(bus_index, phone, email, name);
            } else {
                printf("❌ Invalid bus selection!\n\n");
            }
            
        // Option 4: Cancel booking
        } else if (choice == 4) {
            printf("Enter Booking ID to cancel: ");
            int booking_id;
            scanf("%d", &booking_id);
            cancelBooking(booking_id);
        }
    }
    
    printf("\n👋 Thank you for using SmartBus! Drive Safe 🚌\n");
    return 0;
}
