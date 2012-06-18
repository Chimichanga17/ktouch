/*
 *  Copyright 2012  Sebastian Gottfried <sebastiangottfried@web.de>
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License as
 *  published by the Free Software Foundation; either version 2 of
 *  the License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

import QtQuick 1.1
import org.kde.plasma.core 0.1 as PlasmaCore
import org.kde.plasma.components 0.1 as PlasmaComponents
import ktouch 1.0

Column {
    id: item

    property variant courses;
    property Profile profile;
    signal lessonSelected(variant course, int lessonIndex)

    property variant boxes: [];
    property int lastActiveIndex: -1
    property int activeIndex: 0

    function cellHeight(index)
    {
        if (index == -1)
            return 0
        var box = item.boxes[index]
        if (index == item.activeIndex) {
            var height = item.height;
            for (var i = 0; i < item.boxes.length; i++)
                if (i != index)
                    height -= item.boxes[i].collapsedHeight
            return height
        }
        else {
            return box.collapsedHeight
        }
    }

    function activateBox(index) {
        item.lastActiveIndex = item.activeIndex
        item.activeIndex = index
        boxSwitchAnimation.start()
    }

    function updateBoxes() {
        var boxes = []
        for (var i = 0; i < repeater.count; i++) {
            boxes.push(repeater.itemAt(i))
        }
        item.boxes = boxes;
        lastActiveIndex = -1
        activeIndex = 0
        setBoxHeights()
    }

    function setBoxHeights() {
        for (var i = 0; i < item.boxes.length; i++) {
            item.boxes[i].height = cellHeight(i)
        }
    }

    onHeightChanged: setBoxHeights()

    Repeater {
        id: repeater
        property variant courses: keyboardLayout.correspondingDataIndexCourses
        model: keyboardLayout.isValid? courses.length: 0
        onModelChanged: updateBoxes()

        HomeScreenBox {
            id: box
            width: item.width
            title: i18n(repeater.courses[index].title)
            active: activeIndex === index
            onActivated: activateBox(index)
            content: LessonSelector {
                dataIndexCourse: repeater.courses[index]
                profile: item.profile
                onLessonSelected: item.lessonSelected(course, lessonIndex)
            }
        }
    }

    ParallelAnimation {
        id: boxSwitchAnimation
        NumberAnimation {
            target: item.lastActiveIndex != -1? item.boxes[item.lastActiveIndex]: null
            property: "height"
            to: cellHeight(item.lastActiveIndex)
            duration: 250
        }
        NumberAnimation {
            target: item.boxes[item.activeIndex]
            property: "height"
            to: cellHeight(item.activeIndex)
            duration: 250
        }
    }

    Component.onCompleted: updateBoxes()
}
