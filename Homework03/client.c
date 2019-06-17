#include "sem_service.c"


void init_transaction(struct BankSystem bankSystem, struct User user) {

    memcpy(bankSystem.user, &user, sizeof(struct User));

    post_sem(bankSystem.notify_sem, "notify_sem");

    wait_sem(bankSystem.bank_sem, "bank_sem");

}

int main(int argc, char **argv) {

    validateClientInput(argc, argv);

    char accId = argv[FIRST_ARGUMENT_INDEX][0];

    struct BankSystem bankSystem;

    initClientSystem(&bankSystem);

    struct User user = createUser(accId);

    init_transaction(bankSystem, user);

    printf("Account: %c. Balance: %u. Enter amount: ", accId, bankSystem.user->balance);

    if (scanf("%d", &user.amount) < 0) {
        post_sem(bankSystem.mutex, "mutex");

        error("Failed while reading user amount.");
    }

    init_transaction(bankSystem, user);

    if (bankSystem.user->result == 0) {
        post_sem(bankSystem.mutex, "mutex");

        error("Not enough funds to make the transaction.");
    }

    post_sem(bankSystem.mutex, "mutex");

    printf("Transaction completed successfully.\n");
    exit(0);
}