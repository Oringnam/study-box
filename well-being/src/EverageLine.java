import java.awt.*;
import javax.swing.*;


public class EverageLine extends JPanel{
	//��õ�Ǵ� ������ ������ ���� ������ ǥ��
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