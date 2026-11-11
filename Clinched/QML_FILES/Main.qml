import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts

ApplicationWindow {
    visible: true
    width: 800
    height: 600
    title: "Clinched"

    // Theme: the single place to control colors
    Material.theme: Material.Dark
    Material.accent: Material.Red

    header: TabBar {
        id: tabBar

        TabButton { text: "Dashboard" }
        TabButton { text: "Diet" }
        TabButton { text: "Workouts" }
        TabButton { text: "Profile" }
    }

    StackLayout {
        anchors.fill: parent
        currentIndex: tabBar.currentIndex

        Item { Label { text: "Dashboard page"; anchors.centerIn: parent } }
        DietView {}
        Item { Label { text: "Workouts page"; anchors.centerIn: parent } }
        Item { Label { text: "Profile page"; anchors.centerIn: parent } }
    }
}