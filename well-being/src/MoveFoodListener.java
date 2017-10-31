import java.awt.*;
import javax.swing.*;
import java.awt.event.*;
import java.util.*;
public class MoveFoodListener implements MouseListener 
{	

	JTextField name = new JTextField();
	JTextField tan= new JTextField();
	JTextField fat= new JTextField();
	JTextField protein= new JTextField();
	JTextField cal= new JTextField();
	JButton apply ;
	JButton exit ;
	private  TimePanel time;
	private  FoodButtonList food;

	private  TimeLinePanel t_prev;
	private  TimeLinePanel t_cur;
	private  FoodButton prev;
	private  FoodButton cur;

	private MyListener listener;

	private SecondPanel second;
	private static MoveFoodListener me;
	public static MoveFoodListener getMoveFoodListener(){
		return me;
	}
	public  MoveFoodListener(SecondPanel s){
		me = this;
		listener = new MyListener();
		second = s;
		setTime();
		setFood();
	}
	public void setTime(){
		time = second.getTimePanel();
	}
	public void setFood(){
		food = second.getFoodPanel().getFoodButtonList();
	}
	public void mousePressed(MouseEvent e){}
	public void mouseReleased(MouseEvent e){}
	public void mouseClicked(MouseEvent e)
	{

		if(e.getSource().getClass()==(new TimeLinePanel()).getClass()) //타임라인에서 이벤트 발생
		{
			t_cur = (TimeLinePanel)e.getSource();
			if(t_prev==null) //선택 되었던 타임 라인 존재 하지 않음
			{
				t_cur.setBorder(BorderFactory.createMatteBorder(2,2,2,2,Constant.c2));
			}
			else if(t_cur==t_prev){// 타임라인 지우기
				time.deleteT(t_prev);
				t_prev=null;
				//	t_prev =null;
				//	t_prev.setBorder(BorderFactory.createMatteBorder(2,2,2,2,Color.white));

			}
			else // 타임라인 선택 변경
			{
				for(FoodButtonPanel temp : t_prev.getList()){
					temp.removeMouseListener(listener);
				}
				for(FoodButtonPanel temp : t_cur.getList()){
					temp.addMouseListener(listener);
				}
				t_cur.setBorder(BorderFactory.createMatteBorder(3,3,3,3,Constant.c2));
				t_prev.setBorder(BorderFactory.createMatteBorder(2,2,2,2,Color.white));

			}
			t_prev = t_cur;
		}
		else if(e.getSource().getClass()==(new FoodButton()).getClass())
		{
			cur = (FoodButton)e.getSource();
			if(t_prev!=null){            //타임에 음식추가********************************************
					
				//ImageIcon tempimage = new ImageIcon(Constant.FOODIMAGE[cur.getKey()]);
				
				FoodButtonPanel temp = new FoodButtonPanel(cur );
				//temp.add(new FoodButton(prev));
				//temp.add(new JLabel(cur.getIcon()));
				temp.addMouseListener(listener);
				//temp.removeMouseListener(this);
				t_prev.addF(temp);
				TotalAted.add(cur);
				second.getGraphPanel().startThread();
			
			}
			if(prev==null)
			{
				cur.setBorder(BorderFactory.createMatteBorder(2,2,2,2,Constant.c2));
				prev = cur;				
			}
			else if(cur==prev){					//푸드에서 음식지우기********************************************
			//	food.deleteF(prev);
			//	prev=null;

				//	prev=null;
				//	prev.setBorder(BorderFactory.createMatteBorder(2,2,2,2,Color.white));
				//	time.delete(prev);

			}
			else
			{
				cur.setBorder(BorderFactory.createMatteBorder(3,3,3,3,Constant.c2));
				prev.setBorder(BorderFactory.createMatteBorder(2,2,2,2,Color.white));
				prev = cur;
			}		
		}else if(e.getSource() == second.getOption_time()){
			time.addTimeLine(new TimeLinePanel());
			time.setVisible(false);
			time.setVisible(true);
		}else{
			JDialog popup = new JDialog();
			popup.setTitle("food setup");
			{
				DialogPan pan = new DialogPan(popup);
				popup.setSize(new Dimension(500,750));
				//popup.setLayout(null);
				popup.add(pan);
				popup.setVisible(true);
			}
		}
	}
	public void mouseEntered(MouseEvent e){}
	public void mouseExited(MouseEvent e){}
	//
	private class DialogListener implements ActionListener {
		public void actionPerformed(ActionEvent e){
			if(e.getSource() == apply && prev==null){
				FoodButton temp = new FoodButton();
				second.getFoodPanel().foodAdd(temp);
				second.getFoodPanel().setVisible(false);
				second.getFoodPanel().setVisible(true);
			}
		}
	}
	private class MyListener implements MouseListener{
		public void mousePressed(MouseEvent e){}
		public void mouseReleased(MouseEvent e){}
		public void mouseClicked(MouseEvent e)
		{
			if(t_prev!=null )	// 타임에서 음식제거
			{					
				
				TotalAted.minus((FoodButtonPanel)(e.getSource()));
				t_prev.deleteF((FoodButtonPanel)e.getSource());
				t_prev.setVisible(false);
				t_prev.setVisible(true);
				second.getGraphPanel().startThread();
			}
		}
		public void mouseEntered(MouseEvent e){}
		public void mouseExited(MouseEvent e){}

	}
}