#include "file_handler.c"
#include "sem_service.c"

void fail_transaction(struct User user, struct BankSystem bankSystem) {
    printf("Failed transaction for user: %c\n", user.account);

    user.result = 0;
    memcpy(bankSystem.user, &user, sizeof(struct User));

    post_sem(bankSystem.bank_sem, "bank_sem");
}

void init_transaction(struct BankSystem bankSystem, struct User user) {
    printf("Initiating transaction for user: %c\n", user.account);

    memcpy(bankSystem.user, &user, sizeof(struct User));

    post_sem(bankSystem.bank_sem, "bank_sem");

    wait_sem(bankSystem.notify_sem, "notify_sem");
}

int main(int argc, char *argv[]) {

    validateServerInput(argc);

    uint32_t bank[8] = {0};

    int fd = read_or_default_file(argv[FIRST_ARGUMENT_INDEX], bank);

    struct BankSystem bankSystem;

    printf("Initiating bank system. \n");
    initServerSystem(&bankSystem);
    printf("Bank system successfully initiated. \n");

    while (1) {
        printf("Waiting for input from user.\n");
        wait_sem(bankSystem.notify_sem, "notify_sem");

        struct User user = createUser(bankSystem.user->account);

        int index = user.accIndex;
        user.balance = bank[index];

        init_transaction(bankSystem, user);

        int32_t amount = bankSystem.user->amount;

        if (amount > INT16_MAX || amount < INT16_MIN) {
            fail_transaction(user, bankSystem);
            continue;
        } else {
            int64_t result = (int64_t) bank[index] + (int64_t) amount;
            if (result < 0 || result > UINT32_MAX) {
                fail_transaction(user, bankSystem);
                continue;
            } else {
                user.result = 1;
                bank[index] += amount;

                memcpy(bankSystem.user, &user, sizeof(struct User));

                lseek(fd, 0, SEEK_SET);
                write_bytes_to_fd(fd, bank, FILE_LENGTH);
                printf("Completed transaction successfully for user: %c.\n", user.account);
            }
        }

        post_sem(bankSystem.bank_sem, "bank_sem");
    }

}