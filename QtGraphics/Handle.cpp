#include "Handle.h"
std::vector<QGraphicsItem*> Handle::items;

QPointF Handle::lastPosition;
QPointF  Handle::currentPosition;
float  Handle::lastRotation;
float  Handle::currentRotation;
QSize  Handle::currentScale;

QPointF  Handle::lastMousePosition;
int  Handle::status;

Sprite* Handle::sprite_Handle_PosX;
Sprite* Handle::sprite_Handle_PosY;
QGraphicsItemGroup* Handle::group_Handle_Position;

QGraphicsPathItem* Handle::sprite_Handle_Rotate_Inner;
QGraphicsPathItem* Handle::sprite_Handle_Rotate_Middle;
QGraphicsPathItem* Handle::sprite_Handle_Rotate_Outer;
QGraphicsItemGroup* Handle::group_Handle_Rotation;

QGraphicsLineItem* Handle::sprite_Border_Left_Outer;
QGraphicsLineItem* Handle::sprite_Border_Left_Middle;
QGraphicsLineItem* Handle::sprite_Border_Left_Inner;
QGraphicsItemGroup* Handle::group_Handle_Border_Left;

QGraphicsLineItem* Handle::sprite_Border_Right_Outer;
QGraphicsLineItem* Handle::sprite_Border_Right_Middle;
QGraphicsLineItem* Handle::sprite_Border_Right_Inner;
QGraphicsItemGroup* Handle::group_Handle_Border_Right;

QGraphicsLineItem* Handle::sprite_Border_Top_Outer;
QGraphicsLineItem* Handle::sprite_Border_Top_Middle;
QGraphicsLineItem* Handle::sprite_Border_Top_Inner;
QGraphicsItemGroup* Handle::group_Handle_Border_Top;

QGraphicsLineItem* Handle::sprite_Border_Bottom_Outer;
QGraphicsLineItem* Handle::sprite_Border_Bottom_Middle;
QGraphicsLineItem* Handle::sprite_Border_Bottom_Inner;
QGraphicsItemGroup* Handle::group_Handle_Border_Bottom;

QGraphicsPathItem* Handle::sprite_Point_TopLeft_Outer;
QGraphicsPathItem* Handle::sprite_Point_TopLeft_Middle;
QGraphicsPathItem* Handle::sprite_Point_TopLeft_Inner;
QGraphicsItemGroup* Handle::group_Point_TopLeft;

QGraphicsPathItem* Handle::sprite_Point_TopRight_Outer;
QGraphicsPathItem* Handle::sprite_Point_TopRight_Middle;
QGraphicsPathItem* Handle::sprite_Point_TopRight_Inner;
QGraphicsItemGroup* Handle::group_Point_TopRight;

QGraphicsPathItem* Handle::sprite_Point_BottomLeft_Outer;
QGraphicsPathItem* Handle::sprite_Point_BottomLeft_Middle;
QGraphicsPathItem* Handle::sprite_Point_BottomLeft_Inner;
QGraphicsItemGroup* Handle::group_Point_BottomLeft;

QGraphicsPathItem* Handle::sprite_Point_BottomRight_Outer;
QGraphicsPathItem* Handle::sprite_Point_BottomRight_Middle;
QGraphicsPathItem* Handle::sprite_Point_BottomRight_Inner;
QGraphicsItemGroup* Handle::group_Point_BottomRight;

QGraphicsView* Handle::view;
QGraphicsItem* Handle::hoveredItem;
int Handle::hoverStatus;
bool Handle::itemPressed;
bool Handle::itemHovered;
bool Handle::ctrlPressed;
SizeHandleRect* Handle::rectItem;
SizeHandleRect::Border Handle::border;