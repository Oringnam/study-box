import java.awt.*;
import javax.swing.*;


public class EverageLine extends JPanel{
	//추천되는 정도의 영양을 붉은 선으로 표시
	private double recomanded;
	
	public EverageLine(){}
	public EverageLine(double a)
	{
		setLayout(null);
		setBackground(Color.white);
		recomanded = a;
	}//EverageLine()
	public double getRecomanded()
	{
		return recomanded;
	}//getRecomanded()
}//EveageLine