#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_USERS 100
#define USERNAME_LENGTH 50
#define PASSWORD_LENGTH 50

typedef struct
{
    char username[USERNAME_LENGTH];
    char password[PASSWORD_LENGTH];
    float balance;
    float total_earnings;
    float total_losses;
} User;

// prototypes
int register_user(User *users, int *user_count);
int login_user(User *users, int user_count);
float generate_random_multiplier();
void play_game(User *user);
void save_users(User *users, int user_count);
int load_users(User *users);

int main(void)
{
    srand(time(NULL)); // seed the random number generator
    User users[MAX_USERS];
    int user_count = load_users(users); // load users from file
    int logged_in_user_index = -1;
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
            register_user(users, &user_count); // registering user
            break;
        case 2:
            logged_in_user_index = login_user(users, user_count);
            if (logged_in_user_index != -1)
            {
                play_game(&users[logged_in_user_index]); // playing the game
            }
            break;
        case 3:
            save_users(users, user_count); // saving user to the file
            printf("Had fun? :)\n");
            return 0;
        default:
            printf("Invalid option. Please try again.\n");
        }
    }
}

int register_user(User *users, int *user_count)
{
    if (*user_count >= MAX_USERS)
    {
        printf("User  limit reached.\n");
        return 0;
    }

    User new_user;
    printf("Enter username: ");
    scanf("%s", new_user.username);
    printf("Enter password: ");
    scanf("%s", new_user.password);

    new_user.balance = 10000.0;
    new_user.total_earnings = 0.0;
    new_user.total_losses = 0.0;

    users[*user_count] = new_user;
    (*user_count)++;

    printf("Registration complete. Your initial balance is $%.2f\n", new_user.balance);
    return 1;
}

int login_user(User *users, int user_count)
{
    char username[USERNAME_LENGTH];
    char password[PASSWORD_LENGTH];
    int choice;

    printf("Enter username: ");
    scanf("%s", username);
    printf("Enter password: ");
    scanf("%s", password);

    for (int i = 0; i < user_count; i++)
    {
        if (strcmp(users[i].username, username) == 0 && strcmp(users[i].password, password) == 0)
        {
            printf("Login successful. Welcome, %s.\n", username);
            printf("Your balance is $%.2f\n\n", users[i].balance);
            
            printf("1. Play\n");
            printf("2. Return to Menu\n");
            printf("Choose an option: ");
            scanf("%d", &choice);
            
            if (choice == 1) {
                return i; // return index to play game
            } else {
                return -1; // return to menu
            }
        }
    }

    printf("Invalid username or password.\n");
    return -1; 
}

float generate_random_multiplier()
{
    float random_value = (float)rand() / (float)RAND_MAX; // uniform(0,1)

    if (random_value == 0)
    {
        random_value = 0.0001; 
    }

    return 1.0 / random_value; 
}

void play_game(User *user)
{
    while (1) {
        float bet, cash_out_multiplier;
        float current_multiplier = 1.0;
        float crash_point;
        clock_t start, current;
        int choice;
        int cashed_out = 0;  // flag to track if player cashed out successfully

        printf("Your current balance is: $%.2f\n", user->balance);

        printf("Enter your bet amount: ");
        scanf("%f", &bet);

        if (bet > user->balance)
        {
            printf("You do not have enough balance to place this bet.\n");
            return;
        }

        printf("Enter desired multiplier: ");
        scanf("%f", &cash_out_multiplier);

        crash_point = generate_random_multiplier();
        //printf("The crash point is set at: %.2f\n", crash_point);

        start = clock();
        while (current_multiplier < crash_point)
        {
            printf("Current multiplier: %.2f\n", current_multiplier);
            
            // check if player's multiplier is reached before crash
            if (current_multiplier >= cash_out_multiplier) {
                cashed_out = 1;  // player successfully cashed out
                break;
            }
            
            current = clock();
            while ((double)(clock() - current) / CLOCKS_PER_SEC < 1.0) {
            }
            
            current_multiplier += 0.1;
        }

        printf("The multiplier crashed at %.2f.\n", crash_point);

        if (cashed_out)
        {
            // player cashed out in time
            float winnings = (bet * cash_out_multiplier) - bet;
            user->balance += winnings;
            user->total_earnings += winnings;
            printf("You cashed out at %.2f and won $%.2f. Your new balance is $%.2f\n", 
                   cash_out_multiplier, winnings, user->balance);
        }
        else
        {
            // player didn't cash out in time
            user->balance -= bet;
            user->total_losses += bet;
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

void save_users(User *users, int user_count)
{
    FILE *file = fopen("users.dat", "wb");
    if (file == NULL)
    {
        printf("Error saving user data.\n");
        return;
    }
    fwrite(users, sizeof(User), user_count, file);
    fclose(file);
    printf("User  data saved successfully.\n");
}

int load_users(User *users)
{
    FILE *file = fopen("users.dat", "rb");
    if (file == NULL)
    {
        return 0; // no users found, start with 0
    }
    int user_count = fread(users, sizeof(User), MAX_USERS, file);
    fclose(file);
    printf("Loaded %d users from file.\n", user_count);
    return user_count;
}