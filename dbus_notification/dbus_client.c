#include <gio/gio.h>
#include <stdio.h>

#define BUS_NAME "com.example.Lab2"
#define OBJECT_PATH "/com/example/Lab2"
#define INTERFACE_NAME "com.example.Lab2.Notification"

int main(int argc, char *argv[])
{
    GError *error = NULL;
    GDBusConnection *connection;
    GVariant *result;
    const gchar *response;

    if (argc != 2) {
        printf("Usage: %s <message>\n", argv[0]);
        return 1;
    }

    // 세션 D-Bus에 연결
    connection = g_bus_get_sync(
        G_BUS_TYPE_SESSION,
        NULL,
        &error
    );

    if (connection == NULL) {
        fprintf(stderr, "Connection failed: %s\n",
                error->message);
        g_error_free(error);
        return 1;
    }

    printf("===== D-Bus Notification Client =====\n");
    printf("[Client] Sending notification: %s\n", argv[1]);

    // 서버의 SendNotification 메서드 호출
    result = g_dbus_connection_call_sync(
        connection,
        BUS_NAME,
        OBJECT_PATH,
        INTERFACE_NAME,
        "SendNotification",
        g_variant_new("(s)", argv[1]),
        G_VARIANT_TYPE("(s)"),
        G_DBUS_CALL_FLAGS_NONE,
        5000,
        NULL,
        &error
    );

    if (result == NULL) {
        fprintf(stderr, "[Client] Call failed: %s\n",
                error->message);
        g_error_free(error);
        g_object_unref(connection);
        return 1;
    }

    // 서버 응답 확인
    g_variant_get(result, "(&s)", &response);

    printf("[Client] Server response: %s\n", response);

    g_variant_unref(result);
    g_object_unref(connection);

    return 0;
}
