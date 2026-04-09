#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct clientData
{
    unsigned int acctNum;
    char lastName[15];
    char firstName[10];
    double balance;
};

/* function prototypes */
unsigned int enterChoice(void);
void textFile(FILE *readPtr);
void updateRecord(FILE *fPtr);
void newRecord(FILE *fPtr);
void deleteRecord(FILE *fPtr);
void saveTransaction(unsigned int account, const char type[], double amount);
void showTransactions(void);

int main(void)
{
    FILE *cfPtr;
    unsigned int choice;

    /* open binary file */
    cfPtr = fopen("credit.dat", "rb+");

    if (cfPtr == NULL)
    {
        printf("File could not be opened.\n");
        return 1;
    }

    while ((choice = enterChoice()) != 6)
    {
        switch (choice)
        {
            case 1:
                textFile(cfPtr);
                break;

            case 2:
                updateRecord(cfPtr);
                break;

            case 3:
                newRecord(cfPtr);
                break;

            case 4:
                deleteRecord(cfPtr);
                break;

            case 5:
                showTransactions();
                break;

            default:
                printf("Invalid choice.\n");
        }
    }

    fclose(cfPtr);
    return 0;
}

/* menu */
unsigned int enterChoice(void)
{
    unsigned int choice;

    printf("\n===== BANK MENU =====\n");
    printf("1. Store accounts in text file\n");
    printf("2. Update account (deposit/withdraw)\n");
    printf("3. Add new account\n");
    printf("4. Delete account\n");
    printf("5. Show transaction history\n");
    printf("6. Exit\n");
    printf("Enter choice: ");

    scanf("%u", &choice);

    return choice;
}

/* convert binary to text file */
void textFile(FILE *readPtr)
{
    FILE *writePtr;
    struct clientData client;
    int result;

    writePtr = fopen("accounts.txt", "w");

    if (writePtr == NULL)
    {
        printf("Unable to create text file.\n");
        return;
    }

    rewind(readPtr);

    fprintf(writePtr, "%-6s %-15s %-10s %-10s\n",
            "Acct", "LastName", "FirstName", "Balance");

    while ((result = fread(&client, sizeof(struct clientData), 1, readPtr)) == 1)
    {
        if (client.acctNum != 0)
        {
            fprintf(writePtr, "%-6u %-15s %-10s %.2f\n",
                    client.acctNum,
                    client.lastName,
                    client.firstName,
                    client.balance);
        }
    }

    fclose(writePtr);
    printf("accounts.txt created successfully.\n");
}

/* update record */
void updateRecord(FILE *fPtr)
{
    unsigned int account;
    double transaction;
    struct clientData client = {0, "", "", 0.0};

    printf("Enter account number (1-100): ");
    scanf("%u", &account);

    fseek(fPtr, (account - 1) * sizeof(struct clientData), SEEK_SET);
    fread(&client, sizeof(struct clientData), 1, fPtr);

    if (client.acctNum == 0)
    {
        printf("Account not found.\n");
        return;
    }

    printf("Current balance: %.2f\n", client.balance);
    printf("Enter amount (+ deposit / - withdraw): ");
    scanf("%lf", &transaction);

    client.balance += transaction;

    if (transaction >= 0)
        saveTransaction(account, "Deposit", transaction);
    else
        saveTransaction(account, "Withdraw", transaction);

    fseek(fPtr, -sizeof(struct clientData), SEEK_CUR);
    fwrite(&client, sizeof(struct clientData), 1, fPtr);

    printf("Updated balance: %.2f\n", client.balance);
}

/* add transaction history */
void saveTransaction(unsigned int account, const char type[], double amount)
{
    FILE *tPtr;

    tPtr = fopen("transactions.txt", "a");

    if (tPtr == NULL)
    {
        printf("Cannot open transaction file.\n");
        return;
    }

    fprintf(tPtr, "Account %u - %s : %.2f\n",
            account, type, amount);

    fclose(tPtr);
}

/* show history */
void showTransactions(void)
{
    FILE *tPtr;
    char ch;

    tPtr = fopen("transactions.txt", "r");

    if (tPtr == NULL)
    {
        printf("No transaction history found.\n");
        return;
    }

    printf("\n===== TRANSACTION HISTORY =====\n");

    while ((ch = fgetc(tPtr)) != EOF)
    {
        putchar(ch);
    }

    fclose(tPtr);
}

/* add new record */
void newRecord(FILE *fPtr)
{
    struct clientData client = {0, "", "", 0.0};
    unsigned int accountNum;

    printf("Enter new account number (1-100): ");
    scanf("%u", &accountNum);

    fseek(fPtr, (accountNum - 1) * sizeof(struct clientData), SEEK_SET);
    fread(&client, sizeof(struct clientData), 1, fPtr);

    if (client.acctNum != 0)
    {
        printf("Account already exists.\n");
        return;
    }

    printf("Enter last name first name balance: ");
    scanf("%14s %9s %lf",
          client.lastName,
          client.firstName,
          &client.balance);

    client.acctNum = accountNum;

    fseek(fPtr, (accountNum - 1) * sizeof(struct clientData), SEEK_SET);
    fwrite(&client, sizeof(struct clientData), 1, fPtr);

    printf("Account created successfully.\n");
}

/* delete record */
void deleteRecord(FILE *fPtr)
{
    struct clientData client;
    struct clientData blankClient = {0, "", "", 0.0};
    unsigned int accountNum;

    printf("Enter account number to delete: ");
    scanf("%u", &accountNum);

    fseek(fPtr, (accountNum - 1) * sizeof(struct clientData), SEEK_SET);
    fread(&client, sizeof(struct clientData), 1, fPtr);

    if (client.acctNum == 0)
    {
        printf("Account does not exist.\n");
        return;
    }

    fseek(fPtr, (accountNum - 1) * sizeof(struct clientData), SEEK_SET);
    fwrite(&blankClient, sizeof(struct clientData), 1, fPtr);

    printf("Account deleted successfully.\n");
}