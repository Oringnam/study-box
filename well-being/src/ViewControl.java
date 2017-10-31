import java.awt.*;
import javax.swing.*;
public class ViewControl
{//ȭ�� ��ȯ�� �����ϴ� Ŭ����
	private static Component [] x;
	private static Container contentPane;
	private static FirstPanel first;
	private static SecondPanel second;
	private static ThirdPanel third;
	private static Main main;
	private static StartPanel start;
	
	public static Component getPanel(int i){
		return x[i];
	}//getPanel()
	public ViewControl(Main main)
	{	
		x = new Component[4];
		start = new StartPanel();
		first = new FirstPanel();
		second = new SecondPanel();
		third = new ThirdPanel();
		
		x[0]=first;
		x[1]=second;
		x[2]=third;
		x[3]=start;
		this.main = main;
		init();
	}//ViewControl()
	
	public void init()
	{
		contentPane = main.getContentPane();
		contentPane.add(start);
		main.setVisible(false);
		main.setVisible(true);
	}//init()
	
	public static void viewPanel(int prev, int next)
	{//ȭ����ȯ�� �����ϴ� �Լ�
		contentPane.remove(x[prev-1]);
		contentPane.add(x[next-1]);
		main.setVisible(false);
		main.setVisible(true);	
	}//viewPanel()

}
