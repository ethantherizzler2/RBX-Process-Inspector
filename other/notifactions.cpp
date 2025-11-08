#include "notifications.hpp"
#include <windows.h>

namespace notifications {
    void show(const std::wstring& title, const std::wstring& message) {
        std::wstring command = L"powershell -Command \""
            L"[Windows.UI.Notifications.ToastNotificationManager, Windows.UI.Notifications, ContentType = WindowsRuntime] > $null; "
            L"$template = [Windows.UI.Notifications.ToastTemplateType]::ToastText02; "
            L"$xml = [Windows.UI.Notifications.ToastNotificationManager]::GetTemplateContent($template); "
            L"$textNodes = $xml.GetElementsByTagName('text'); "
            L"$textNodes.Item(0).AppendChild($xml.CreateTextNode('" + title + L"')) > $null; "
            L"$textNodes.Item(1).AppendChild($xml.CreateTextNode('" + message + L"')) > $null; "
            L"$toast = [Windows.UI.Notifications.ToastNotification]::new($xml); "
            L"[Windows.UI.Notifications.ToastNotificationManager]::CreateToastNotifier('Roblox').Show($toast)"
            L"\"";

        _wsystem(command.c_str());
    }
}