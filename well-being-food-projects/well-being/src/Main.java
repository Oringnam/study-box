import java.awt.*;
import javax.swing.*;

public class Main extends JFrame {	
	ViewControl controller;
	public Main(String title){
		super(title);
	        //��������� â�� ��ġ�� ����ش�.
		this.setLayout(null);
        this.setLocation(new Point(100,0));
        //�������� �����ش�. default = false
        this.setVisible(true);
        //�������� âũ�⸦ �����Ѵ�.
        this.setPreferredSize(new Dimension(800,1000));
        //������Ʈ ũ�⸸ŭ â�� ũ�⸦ ����ش�.
        this.setResizable(false);
        this.pack();
        //������â�� ���� ��� ���μ����� �����ϱ� ���� ����ؾ��Ѵ�. 
        this.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        //����Ʈ�ѷ� ���
        
        controller = new ViewControl(this);
	}
	public static void main(String[] args) {
		new Main("Diet");
	}
}