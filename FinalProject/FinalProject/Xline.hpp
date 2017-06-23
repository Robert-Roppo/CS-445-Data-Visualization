#pragma once

struct Vector2f {
public:
	Vector2f() {};

	Vector2f(float x, float y) : x(x), y(y) {};

	float x;
	float y;

	Vector2f operator+(Vector2f b) {
		return Vector2f(x + b.x, y + b.y);
	}
	Vector2f operator+(float b) {
		return Vector2f(x + b, y + b);
	}
	Vector2f operator-(Vector2f b) {
		return Vector2f(x - b.x, y - b.y);
	}
	friend Vector2f operator*(float val, Vector2f a) {
		return Vector2f(a.x*val, a.y*val);
	}
	friend Vector2f operator*(Vector2f a,float val) {
		return Vector2f(a.x*val, a.y*val);
	}
	Vector2f operator*(Vector2f b) {
		return Vector2f(x*b.x,y*b.y);
	}
};

class Xline {
public:
	Vector2f head;
	Vector2f tail;

	void setHead(float headX, float headY) {
		head.x = headX;
		head.y = headY;
	}
	void setTail(float tailX, float tailY) {
		tail.x = tailX;
		tail.y = tailY;
	}
	void addHead(Vector2f vec) {
		head.x += vec.x;
		head.y += vec.y;
	}
	void addTail(Vector2f vec) {
		tail.x += vec.x;
		tail.y += vec.y;
	}

	Xline(Vector2f head, Vector2f tail) {
		setHead(head.x, head.y);
		setTail(tail.x, tail.y);
	}

	Vector2f findGlobalPoint(float normPoint) {
		return (head - tail) * normPoint + tail;
	}


};