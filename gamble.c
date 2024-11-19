#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_USERS 100
#define USERNAME_LEN 50
#define PASSWORD_LEN 50

typedef struct
{
    char uname[USERNAME_LEN];
    char pass[PASSWORD_LEN];
    float balance;
    float earnings;
    float losses;
} User;

// prototypes
int reg(User *users, int *count);
int login(User *users, int count);
float gen_mult();
void play(User *user);
void save_users(User *users, int count);
int load_users(User *users);

int main(void)
{
    srand(time(NULL)); // seed the random number generator
    User users[MAX_USERS];
    int count = load_users(users); // load users from file
    int logged_in_idx = -1;
    int choice;

    while (1)
    {
        printf("\n1. Register\n");
        printf("2. Login\n");
        printf("3. Exit\n");
        printf("Choose an option: ");
        scanf("%d", &choice);

        switch (choice)
        {
        case 1:
            reg(users, &count); // register a new user
            break;
        case 2:
            logged_in_idx = login(users, count);
            if (logged_in_idx != -1)
            {
                play(&users[logged_in_idx]); // start the game for logged-in user
            }
            break;
        case 3:
            save_users(users, count); // save users to the file
            printf("Had fun? :)\n");
            return 0;
        default:
            printf("Invalid option. Please try again.\n");
        }
    }
}

int reg(User *users, int *count)
{
    if (*count >= MAX_USERS)
    {
        printf("User limit reached.\n");
        return 0;
    }

    User new_user;
    printf("Enter username: ");
    scanf("%s", new_user.uname);
    printf("Enter password: ");
    scanf("%s", new_user.pass);

    new_user.balance = 10000.0;
    new_user.earnings = 0.0;
    new_user.losses = 0.0;

    users[*count] = new_user;
    (*count)++;

    printf("Registration complete. Your initial balance is $%.2f\n", new_user.balance);
    return 1;
}

int login(User *users, int count)
{
    char uname[USERNAME_LEN];
    char pass[PASSWORD_LEN];
    int choice;

    printf("Enter username: ");
    scanf("%s", uname);
    printf("Enter password: ");
    scanf("%s", pass);

    for (int i = 0; i < count; i++)
    {
        if (strcmp(users[i].uname, uname) == 0 && strcmp(users[i].pass, pass) == 0)
        {
            printf("Login successful. Welcome, %s.\n", uname);
            printf("Your balance is $%.2f\n\n", users[i].balance);
            
            printf("1. Play Game\n");
            printf("2. Return to Menu\n");
            printf("Choose an option: ");
            scanf("%d", &choice);
            
            if (choice == 1) {
                return i; // return index to play the game
            } else {
                return -1; // return to the menu
            }
        }
    }

    printf("Invalid username or password.\n");
    return -1; 
}

float gen_mult()
{
    float rand_val = (float)rand() / (float)RAND_MAX; // uniform(0,1)

    if (rand_val == 0)
    {
        rand_val = 0.0001; 
    }

    return 1.0 / rand_val; 
}

void play(User *user)
{
    while (1) {
        float bet, cash_out;
        float mult = 1.0;
        float crash_point;
        clock_t start, current;
        int choice;
        int cashed_out = 0;  // flag to track if the player cashed out successfully

        printf("Your current balance is: $%.2f\n", user->balance);

        printf("Enter your bet amount: ");
        scanf("%f", &bet);

        if (bet > user->balance)
        {
            printf("You do not have enough balance to place this bet.\n");
            return;
        }

        printf("Enter desired multiplier to cash out: ");
        scanf("%f", &cash_out);

        crash_point = gen_mult();
        //printf("The crash point is set at: %.2f\n", crash_point);

        start = clock();
        while (mult < crash_point)
        {
            printf("Current multiplier: %.2f\n", mult);
            
            // check if player's multiplier is reached before crash
            if (mult >= cash_out) {
                cashed_out = 1;  // player successfully cashed out
                break;
            }
            
            current = clock();
            while ((double)(clock() - current) / CLOCKS_PER_SEC < 1.0) {
            }
            
            mult += 0.1;
        }

        printf("The multiplier crashed at %.2f.\n", crash_point);

        if (cashed_out)
        {
            // player cashed out in time
            float winnings = (bet * cash_out) - bet;
            user->balance += winnings;
            user->earnings += winnings;
            printf("You cashed out at %.2f and won $%.2f. Your new balance is $%.2f\n", 
                   cash_out, winnings, user->balance);
        }
        else
        {
            // player didn't cash out in time
            user->balance -= bet;
            user->losses += bet;
            printf("You lost your bet of $%.2f. Your new balance is $%.2f\n", 
                   bet, user->balance);
        }

        printf("\n1. Continue?\n");
        printf("2. Return to menu\n");
        printf("Choose an option: ");
        scanf("%d", &choice);

        if (choice != 1) {
            return;
        }
    }
}

void save_users(User *users, int count)
{
    FILE *file = fopen("users.dat", "wb");
    if (file == NULL)
    {
        printf("Error saving user data.\n");
        return;
    }
    fwrite(users, sizeof(User), count, file);
    fclose(file);
    printf("User data saved successfully.\n");
}

int load_users(User *users)
{
    FILE *file = fopen("users.dat", "rb");
    if (file == NULL)
    {
        return 0; // no users found, start with 0
    }
    int count = fread(users, sizeof(User), MAX_USERS, file);
    fclose(file);
    printf("Loaded %d users from file.\n", count);
    return count;
}
