import java.awt.*;
import javax.swing.*;

public class FoodAteBar extends JPanel {		
			
	private int destiny,delay,i;
	private double data,dataD;	
	private int rectX,percent,prePer;
	
	private EverageLine eLine;
	private GraphPanel primary;
	
	public FoodAteBar(double a, double b)
	{
		setBackground(Color.white);	

		i=0;
		eLine = new EverageLine(630/1.5);	// ���差 ���ݼ�
		delay = 15;		// thread speed
	}	// FoodAteBar()
	
	public void renew(int x, int y, double a, double b){
		rectX = x;	// ���� �׷���
		prePer = y;	// ���� �ۼ�Ʈ
		data = a;
		dataD = b;		
		destiny = (int)((data*630)/(dataD*1.5));	// �׷����� ��ǥ��
		percent = (int)	((data/dataD)*100);	// �ۼ�Ʈ�� ��ǥ��
		
		new GraphThread(this).start();	
	}	// renew()
	
	
	// get / set
	public int getDestiny(double a, double b)	{ 
		destiny = (int)((a*630)/(b*1.5));
		return destiny; 
	}	// getDestiny()
	public int getPercent() {
		if (i<0) i = 0;
		return i;
	}	// getPercent()
	
	public void setThis(GraphPanel p)
	{
		primary = p;	
	}	// setThis()
	
	public void paintComponent(Graphics page)
	{
		super.paintComponent(page);
		
		page.setColor(new Color(120,170,175));
		page.fillRect(0,5,rectX,30);	
		
		page.setColor(Constant.c2);
		page.drawLine((int)eLine.getRecomanded(), 0, (int)eLine.getRecomanded(), 40);		
	}	// paintComponent();
	

		// thread
	class  GraphThread extends Thread
	{
 		FoodAteBar graph;
 	 	
 		public GraphThread(FoodAteBar p){
			graph = p;
 		} 
 		public void run(){
 			// percent counting
 			if(percent > prePer) {	// �Է� �� 
 				for (i = prePer; i<=percent ; i++){
 					primary.countD();
 					try{
 	   					 Thread.sleep(delay*2);
 	   				}catch(Exception e){}
 				}	
 			} else if(percent < prePer){	// ���� ��
				for (i = prePer; i>=percent ; i--){
 					primary.countD();
 					try{
	   					 Thread.sleep(delay*2);
	   				}catch(Exception e){}
				}	  	
 			}	// if.. else
 			
 			// graph
 			while(destiny > rectX){	// �Է��� ��
  				rectX += 1;
				graph.repaint();				  		  
   				try{
   					 Thread.sleep(delay);
   				}catch(Exception e){}
  			} 	// while
 			while(destiny < rectX){	// ������ ��
  				rectX -= 1;
				graph.repaint();	
   				try{
   					 Thread.sleep(delay);
   				}catch(Exception e){}
  			} 	// while
 		} 	// run() 		
	}	// GraphThread()
	
}	// FoodAteBar class