#include <gio/gio.h>
#include <stdio.h>
#define BUS_NAME "com.example.Lab2"
#define OBJECT_PATH "/com/example/Lab2"
#define INTERFACE_NAME "com.example.Lab2.Notification"
// D-Bus 인터페이스 정의
static const gchar introspection_xml[] =
    "<node>"
    "  <interface name='com.example.Lab2.Notification'>"
    "    <method name='SendNotification'>"
    "      <arg type='s' name='message' direction='in'/>"
    "      <arg type='s' name='response' direction='out'/>"
    "    </method>"
    "  </interface>"
    "</node>";
static GDBusNodeInfo *introspection_data = NULL;
static GMainLoop *main_loop = NULL;
// 클라이언트의 메서드 호출 처리
static void handle_method_call(
    GDBusConnection *connection,
    const gchar *sender,
    const gchar *object_path,
    const gchar *interface_name,
    const gchar *method_name,
    GVariant *parameters,
    GDBusMethodInvocation *invocation,
    gpointer user_data)
{
    (void)connection;
    (void)sender;
    (void)object_path;
    (void)interface_name;
    (void)user_data;
    if (g_strcmp0(method_name, "SendNotification") == 0) {
        const gchar *message;
        g_variant_get(parameters, "(&s)", &message);
        printf("[Server] Notification received: %s\n", message);
        // 클라이언트에 응답 전송
        g_dbus_method_invocation_return_value(
            invocation,
            g_variant_new("(s)", "Notification received successfully")
        );
    }
}
// D-Bus 메서드 등록
static const GDBusInterfaceVTable interface_vtable = {
    .method_call = handle_method_call
};
// 버스 연결 시 호출
static void on_bus_acquired(
    GDBusConnection *connection,
    const gchar *name,
    gpointer user_data)
{
    (void)name;
    (void)user_data;

    GError *error = NULL;

    guint registration_id =
        g_dbus_connection_register_object(
            connection,
            OBJECT_PATH,
            introspection_data->interfaces[0],
            &interface_vtable,
            NULL,
            NULL,
            &error
        );

    if (registration_id == 0) {
        fprintf(stderr, "Registration failed: %s\n",
                error->message);
        g_error_free(error);
        g_main_loop_quit(main_loop);
        return;
    }

    printf("[Server] D-Bus object registered.\n");
}

// 버스 이름 획득 시 호출
static void on_name_acquired(
    GDBusConnection *connection,
    const gchar *name,
    gpointer user_data)
{
    (void)connection;
    (void)user_data;

    printf("[Server] Bus name acquired: %s\n", name);
    printf("[Server] Waiting for notifications...\n");
}

// 버스 이름 획득 실패 시 호출
static void on_name_lost(
    GDBusConnection *connection,
    const gchar *name,
    gpointer user_data)
{
    (void)connection;
    (void)user_data;
    fprintf(stderr, "[Server] Bus name lost: %s\n", name);
    g_main_loop_quit(main_loop);
}
int main(void)
{
    GError *error = NULL;
    introspection_data =
        g_dbus_node_info_new_for_xml(
            introspection_xml, &error);
    if (introspection_data == NULL) {
        fprintf(stderr, "XML parsing failed: %s\n",
                error->message);
        g_error_free(error);
        return 1;
    }
    main_loop = g_main_loop_new(NULL, FALSE);
    // 세션 D-Bus에 서버 이름 등록
    guint owner_id = g_bus_own_name(
        G_BUS_TYPE_SESSION,
        BUS_NAME,
        G_BUS_NAME_OWNER_FLAGS_NONE,
        on_bus_acquired,
        on_name_acquired,
        on_name_lost,
        NULL,
        NULL
    );
    printf("===== D-Bus Notification Server =====\n");
    // 메시지 수신 대기
    g_main_loop_run(main_loop);
    g_bus_unown_name(owner_id);
    g_main_loop_unref(main_loop);
    g_dbus_node_info_unref(introspection_data);
    return 0;
}
