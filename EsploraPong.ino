



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

void movePaddle(Paddle& paddle, int8_t movement);
void displayPaddle(Paddle& paddle);
void clearPaddle(Paddle& paddle);

void moveBall(Ball& ball);
void displayBall(Ball& ball);
void clearBall(Ball& ball);
void checkPaddleBallCollision(Paddle& paddle, Ball& ball);




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

int buttons[2] = { SWITCH_DOWN, SWITCH_UP };

int8_t normalizeButtonInput()
{
	byte input = 0;
	for (int i = 0; i < 2; i++)
	{
		if (Esplora.readButton(buttons[i]) == LOW)
		{
			input = buttons[i];
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

	if (paddle.posX == ball.posX + offsetX)
	{
		if (paddle.posY <= ball.posY + ball.radius
			&& paddle.posY + paddle.lenght >= ball.posY + ball.radius)
			ball.speedX *= -1;
	}
}




void setup() {
  // put your setup code here, to run once:
	rightPaddle.posX = 150;
	rightPaddle.color = 0xFF00FF;

	screen.begin();
	screen.background(0x00);
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
	

	//display to user
	displayPaddle(leftPaddle);
	displayPaddle(rightPaddle);
	displayBall(ball1);

	//frame timing
	delay(map(Esplora.readSlider(), 0, 1023, 5, 25));

	//clear screen
	clearPaddle(leftPaddle);
	clearPaddle(rightPaddle);
	clearBall(ball1);


}
