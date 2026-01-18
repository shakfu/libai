#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ai.h"

int main(void) {
    printf("=== libai Basic Test ===\n\n");

    // Initialize library
    printf("Initializing library...\n");
    ai_result_t result = ai_init();
    if (result != AI_SUCCESS) {
        printf("FAIL: ai_init() returned %d: %s\n", result, ai_get_error_description(result));
        return 1;
    }
    printf("OK: Library initialized\n");

    // Check version
    const char *version = ai_get_version();
    printf("OK: Library version: %s\n", version);

    // Check availability
    printf("\nChecking Apple Intelligence availability...\n");
    ai_availability_t availability = ai_check_availability();
    char *reason = ai_get_availability_reason();
    printf("Availability status: %d\n", availability);
    if (reason) {
        printf("Reason: %s\n", reason);
        ai_free_string(reason);
    }

    if (availability != AI_AVAILABLE) {
        printf("\nSKIP: Apple Intelligence not available on this device\n");
        printf("This is expected if running on unsupported hardware or if AI is not enabled.\n");
        ai_cleanup();
        return 0;
    }

    // Create context
    printf("\nCreating context...\n");
    ai_context_t *ctx = ai_context_create();
    if (!ctx) {
        printf("FAIL: ai_context_create() returned NULL\n");
        ai_cleanup();
        return 1;
    }
    printf("OK: Context created\n");

    // Create session
    printf("\nCreating session...\n");
    ai_session_id_t session = ai_create_session(ctx, NULL);
    if (session == AI_INVALID_ID) {
        printf("FAIL: ai_create_session() returned AI_INVALID_ID\n");
        printf("Error: %s\n", ai_get_last_error(ctx));
        ai_context_free(ctx);
        ai_cleanup();
        return 1;
    }
    printf("OK: Session created with ID %d\n", session);

    // Generate response
    printf("\nGenerating response to 'Hello'...\n");
    char *response = ai_generate_response(ctx, session, "Hello", NULL);
    if (!response) {
        printf("FAIL: ai_generate_response() returned NULL\n");
        printf("Error: %s\n", ai_get_last_error(ctx));
        ai_context_free(ctx);
        ai_cleanup();
        return 1;
    }
    printf("OK: Response received\n");
    printf("Response: %s\n", response);
    ai_free_string(response);

    // Get session history
    printf("\nGetting session history...\n");
    char *history = ai_get_session_history(ctx, session);
    if (history) {
        printf("OK: History retrieved (%zu bytes)\n", strlen(history));
        ai_free_string(history);
    } else {
        printf("WARN: Could not retrieve history\n");
    }

    // Get stats
    ai_stats_t stats;
    if (ai_get_stats(ctx, &stats) == AI_SUCCESS) {
        printf("\nStats: total=%llu, successful=%llu, failed=%llu\n",
               stats.total_requests, stats.successful_requests, stats.failed_requests);
    }

    // Cleanup
    printf("\nCleaning up...\n");
    ai_context_free(ctx);
    ai_cleanup();
    printf("OK: Cleanup complete\n");

    printf("\n=== All tests passed ===\n");
    return 0;
}
