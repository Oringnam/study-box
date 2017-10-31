import java.awt.*;
import javax.swing.*;

public class Main extends JFrame {	
	ViewControl controller;
	public Main(String title){
		super(title);
	        //윈도우상의 창의 위치를 잡아준다.
		this.setLayout(null);
        this.setLocation(new Point(100,0));
        //프레임을 보여준다. default = false
        this.setVisible(true);
        //프레임의 창크기를 설정한다.
        this.setPreferredSize(new Dimension(800,1000));
        //컴포넌트 크기만큼 창의 크기를 잡아준다.
        this.setResizable(false);
        this.pack();
        //프레임창을 닫을 경우 프로세스를 종료하기 위해 사용해야한다. 
        this.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        //뷰컨트롤러 등록
        
        controller = new ViewControl(this);
	}
	public static void main(String[] args) {
		new Main("Diet");
	}
}