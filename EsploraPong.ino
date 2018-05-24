



#include <TFT.h>
#include <SPI.h>
#include <Esplora.h>

TFT screen = TFT(7, 0, 1);

struct Paddle 
{
	byte posX = 5;
	byte posY = 5;
	byte width = 2;
	byte lenght = 20;
	byte score = 0;
	int color = 0xFFFF00;
};

struct Ball
{
	byte posX = screen.width() / 2;
	byte posY = screen.height() / 2;
	byte radius = 2;

	int8_t speedX = 1;
	int8_t speedY = 1;

	int color = 0xFFFFFF;
	

};

Paddle leftPaddle;
Paddle rightPaddle;
Ball ball1;

int movementButtons[2] = { SWITCH_DOWN, SWITCH_UP };

void movePaddle(Paddle& paddle, int8_t movement);
void displayPaddle(Paddle& paddle);
void clearPaddle(Paddle& paddle);

void moveBall(Ball& ball);
void displayBall(Ball& ball);
void clearBall(Ball& ball);
void resetBall(Ball& ball);
bool isBallOut(Ball& ball);

void checkPaddleBallCollision(Paddle& paddle, Ball& ball);

void displayScore();


int8_t normalizeJoystickInput()
{
	if (Esplora.readJoystickY() > 100)
	{
		return 1;
	}
	else if (Esplora.readJoystickY() < -100)
	{
		return -1;
	}
	else
	{
		return 0;
	}

}

int8_t normalizeButtonInput()
{
	byte input = 0;
	for (int i = 0; i < 2; i++)
	{
		if (Esplora.readButton(movementButtons[i]) == LOW)
		{
			input = movementButtons[i];
		}
	}

	switch (input)
	{
	case SWITCH_UP:
		return -1;
		break;
	case SWITCH_DOWN:
		return 1;
		break;
	default:
		return 0;
		break;
	}
}

void movePaddle(Paddle& paddle, int8_t movement)
{
	if (paddle.posY + movement < 0 //check whether the paddle is heading out of bounds
	|| ((paddle.posY + paddle.lenght) + movement) > screen.height()) return;

	paddle.posY += movement;
}

void moveBall(Ball& ball)
{
	ball.posX += ball.speedX;

	if (ball.posY + ball.radius <= 0 || ball.posY + ball.radius >= screen.height())
	{
		ball.speedY *= -1;
	}
	ball.posY += ball.speedY;
}



void displayPaddle(Paddle& paddle)
{
	screen.fillRect(paddle.posX, paddle.posY, paddle.width, paddle.lenght, paddle.color);
}

void clearPaddle(Paddle& paddle)
{
	screen.fillRect(paddle.posX, paddle.posY, paddle.width, paddle.lenght, 0x00);
}

void displayBall(Ball& ball)
{
	screen.fillCircle(ball.posX, ball.posY, ball.radius, ball.color);
}

void clearBall(Ball& ball)
{
	screen.fillCircle(ball.posX, ball.posY, ball.radius, 0x00);
}

void resetBall(Ball& ball)
{
	delay(500);
	ball.posX = screen.width() / 2;
	ball.posY = screen.height() / 2;
}


void checkPaddleBallCollision(Paddle& paddle, Ball& ball)
{
	//offset allows to react appropriately to the paddles width (as in the ball will not "sink into" the paddle, 
	//while still being able to use one function for both paddles

	int offsetX = 0;

	if (paddle.posX > screen.width() / 2)
	{
		offsetX = 2;
	}
	else
	{
		offsetX = -2;
	}

	if (paddle.posX == ball.posX + offsetX) //if the ball is on the same X axis as the paddle
	{
		if (paddle.posY <= ball.posY + ball.radius
			&& paddle.posY + paddle.lenght >= ball.posY + ball.radius) //if the ball is between paddle start and end
		{
			ball.speedX *= -1; 
		}
	}
}

bool isBallOut(Ball& ball)
{
	if (ball.posX >= screen.width())
	{
		leftPaddle.score++;
		return true;
	}
	else if (ball.posX <= 0)
	{
		rightPaddle.score++;
		return true;
	}
	else return false;
}


void displayScore()
{
	char printoutLeft[4];
	char printoutRight[4];

	String rightScore = String(rightPaddle.score);
	String leftScore = String(leftPaddle.score);

	screen.stroke(0, 0, 0);
	screen.text(printoutRight, 95, 120);
	screen.text(printoutLeft, 65, 120);

	screen.stroke(255, 255, 255);
	rightScore.toCharArray(printoutRight, 4);
	screen.text(printoutRight, 95, 120);

	leftScore.toCharArray(printoutLeft, 4);
	screen.text(printoutLeft, 65, 120);

	
}


void setup() {
  // put your setup code here, to run once:
	rightPaddle.posX = 150;
	rightPaddle.color = 0xFF00FF;

	screen.begin();
	screen.background(0x00);
	screen.stroke(255, 255, 255);
	screen.setTextSize(1);
	delay(1000);

}

void loop() {
  // put your main code here, to run repeatedly:

	//get input from user
	movePaddle(leftPaddle, normalizeJoystickInput() * 2);
	movePaddle(rightPaddle, normalizeButtonInput() * 2);
	moveBall(ball1);

	//physics
	checkPaddleBallCollision(leftPaddle, ball1);
	checkPaddleBallCollision(rightPaddle, ball1);

	if (isBallOut(ball1))
	{
		resetBall(ball1);
		displayScore();
	}
	

	//display to user
	displayPaddle(leftPaddle);
	displayPaddle(rightPaddle);
	displayBall(ball1);
	

	//frame timing based on potentiometer slider
	delay(map(Esplora.readSlider(), 0, 1023, 5, 25));

	//clear screen
	clearPaddle(leftPaddle);
	clearPaddle(rightPaddle);
	clearBall(ball1);



}
