program integral;
  type TF=function(x:real):real;
  var square1,square2,square3,squaretest1,squaretest2,squaretest3,root12,
      root23,root13,roottest,eps1,eps2,s,leftr,rightr,lefts1,lefts2,lefts3,rights3,
      rights1,rights2,p:real;
      debugmode,stepr,stepi,segments,squaremode,t:integer;
      
    {функции}
    function f1(x:real):real;
      begin
        f1:=ln(x);
      end;
    function f2(x:real):real;
      begin
        f2:=1/(2-x)+6;
      end;
    function f3(x:real):real;
      begin
        f3:=-2*x+14;
      end;
    function ftest1(x:real):real;//тестовая
      begin
        ftest1:=sqr(x);  //именно с такими функциями (для левого и правого корня) хорошо видна разница между 1й и 2й ситуацией
      end;
    function ftest2(x:real):real;//тестовая
      begin
        ftest2:=5*x-6;
      end;
    function ftest3(x:real):real;//тестовая, нужна, чтобы не заводить отдельно функцию для подсчета площади
      begin
        ftest3:=x*x*x+5;
      end;
      
      {производные}
    function f1p(x:real):real;
      begin
        f1p:=1/x;
      end;
    function f2p(x:real):real;
      begin
        f2p:=1/sqr(2-x);
      end;
    function f3p(x:real):real;
      begin
        f3p:=-2;
      end;
    function ftestp1(x:real):real;//тестовая
      begin
        ftestp1:=2*x;
      end;
    function ftestp2(x:real):real;//тестовая
      begin
        ftestp2:=5;
      end;
      {разница функций}
    function dif(f0,g0:TF;x:real):real;
      begin
        dif:=f0(x)-g0(x);
      end;
      {корни}
    function root(f0,g0,f1,g1:TF;left,right:real):real;
        var c,d,a,b:real;
      begin
        if (stepr=1) and ((debugmode=2) or(debugmode=3)) then
          writeln('Step: 1 left= ',left:t:t,' right= ',right:t:t,' difference= ',(right-left):t:t);
        a:=left;
        b:=right;
          //проверка условий из текста задачи
        if (b-a)>=eps1 then //если вдруг сразу задан подходящий интервал
          begin
            stepr:=stepr+1;
            if (dif(f0,g0,a)>0)and(dif(f0,g0,(a+b)/2)>(dif(f0,g0,a)+dif(f0,g0,b))/2) or 
                 //>0-убывает        >-график выше хорды 
            (dif(f0,g0,a)<0)and(dif(f0,g0,(a+b)/2)<(dif(f0,g0,a)+dif(f0,g0,b))/2) then 
                 //<0-возрастает         <-график ниже хорды
              begin
                c:=(a*dif(f0,g0,b)-b*dif(f0,g0,a))/(dif(f0,g0,b)-dif(f0,g0,a));
                d:=b-dif(f0,g0,b)/dif(f1,g1,b);//первый случай
                if (debugmode=2) or (debugmode=3) then
                  writeln('Step: ',stepr,' left= ',c:t:t,' right= ',d:t:t,' difference= ',(d-c):t:t, ' situation: 1');
              end 
            else
              begin
                c:=a-dif(f0,g0,a)/dif(f1,g1,a);//второй случай
                d:=(a*dif(f0,g0,b)-b*dif(f0,g0,a))/(dif(f0,g0,b)-dif(f0,g0,a));
                if (debugmode=2) or (debugmode=3) then
                  writeln('Step: ',stepr,' left= ',c:t:t,' right= ',d:t:t,' difference= ',(d-c):t:t, ' situation: 2');
              end;
            if (d-c)<eps1 then root:=(d+c)/2
            else root:=root(f0,g0,f1,g1,c,d);
          end;
        end;
        
    {площади}
    function square(f0:TF;l,r:real):real;
      var d,s1,s2:real;
          i:integer;
    begin
      d:=r-l;
      s1:=f0(l)*d;  //вычисляем начальное значение для одного сегмента(разбиений 2^0)
      d:=d/2;
      s2:=(f0(l)+f0(l+d))*d; //вычисляем начальное значение для двух сегментов(разбиений 2^1)
      if (debugmode=2) or (debugmode=3) then 
        writeln('Step: ',stepi,' segments count: ',segments,' square= ',s1:t:t);//первый шаг, сравнивать площади не с чем
      segments:=segments*2;
      stepi:=stepi+1;
      if (debugmode=2) or (debugmode=3) then
        writeln('Step: ',stepi,' segments count: ',segments,' square= ',s2:t:t,' difference= ',abs(s2-s1):t:t);
      while (p)*abs(s2-s1)>=eps2 do // 1/3 из текста задачи
        begin
          d:=d/2;  //уменьшаем сегменты в два раза
          segments:=segments*2;//увеличиваем количество сегментов в 2 раза
          s1:=s2;  //предыдущее значение для сравнения, 2^(n) сегментов
          stepi:=stepi+1;  //увеличиваем число посчитанных шагов (они считаются с 1 т.к. начальное разбиение тоже считается)
          s2:=0;
          for i:=0 to segments-1 do  //считаем новое значение, уже для 2^(n+1) сегментов
            s2:=s2+(f0(l+i*d))*d;  //последовательно прибавляем к значению в левой грани (поэтому от 0) 
                                   //значения площади сегментов (крайние правый не нужно. Пример:
                                   //разбиение на 2 части. Считаем значение слева и в середине
          if (debugmode=2) or (debugmode=3) then
          writeln('Step: ',stepi,' segments count: ',segments,' square= ',s2:5:5,' difference= ',abs(s2-s1):5:5);
        end;
        square:=s2;
    end;
    
begin
  t:=5;
  p:=1;
  eps1:=0.001; //чтобы были значения "по умолчанию"
  eps2:=0.001;
  writeln('Intut eps1(for roots)');
  readln(eps1);
  writeln('Input eps2(for squares)');
  readln(eps2);
  segments:=1;
  stepr:=1;  //step root
  stepi:=1;  //step integral
  writeln('write the program mode (1-standart,2-debug,3-test)');
  read(debugmode);
  writeln('____________________');
  if debugmode<>3 then
    begin
      writeln('Roots:');
      if debugmode=2 then
        writeln('Root(F1=ln(x))');
      root12:=root(f1,f2,f1p,f2p,2.01,3);
        writeln('Root f1-f2 = ',root12:t:t,
        ' with ',stepr, ' steps');
        stepr:=1;
      if debugmode=2 then 
        begin
          writeln('___________________');
          writeln('Root(F2=1/(2-x)+6)');
        end;
      root23:=root(f2,f3,f2p,f3p,4,5);
        writeln('Root f2-f3 = ',root23:t:t,
          ' with ',stepr, ' steps');
        stepr:=1;
        if debugmode=2 then 
          begin
            writeln('____________________');
            writeln('Root(F3=-2x+14)');
          end;
      root13:=root(f1,f3,f1p,f3p,5,7);
        writeln('Root f1-f3 = ',root13:t:t,
        ' with ',stepr, ' steps');
      writeln('____________________');
      writeln('Squares:');
      if debugmode=2 then 
        begin
          writeln('____________________');
          writeln('Square(F1=ln(x))');
        end;
      square1:=square(f1,root12,root13);
        writeln('Square(F1=ln(x)) from ',root12:t:t,' to ',root13:t:t,' = ',square1:t:t,
          ' with ',stepi, ' steps and ',segments, ' segments');
        stepi:=1;
        segments:=1;
      if debugmode=2 then 
        begin
          writeln('____________________');
          writeln('Square(F2=1/(2-x)+6)');
        end;
      square2:=square(f2,root12,root23);
        writeln('Square(F2=1/(2-x)+6) from ',root12:t:t,' to ',root23:t:t,' = ',square2:t:t,
          ' with ',stepi, ' steps and ',segments, ' segments');
        stepi:=1;
        segments:=1;
      if debugmode=2 then 
        begin
          writeln('____________________');
          writeln('Square(F3=-2x+14)');
        end;
      square3:=square(f3,root23,root13);
        writeln('Square(F3=-2x+14) from ',root23:t:t,' to ',root13:t:t,' = ',square3:t:t,
          ' with ',stepi, ' steps and ',segments, ' segments');
        writeln('____________________');
        stepi:=1;
        segments:=1;
      s:=square2+square3-square1;
      writeln('Square= ',s:t:t);
    end
  else
    begin
      writeln('Input the segment(root)');
      read(leftr,rightr);
      writeln('Root (ftest1-ftest2) on  (',leftr:t:t,' ',rightr:t:t,')');
      roottest:=root(ftest1,ftest2,ftestp1,ftestp2,leftr,rightr);
      writeln('Root (ftest1-ftest2) = ',roottest:t:t,' from ',leftr:t:t,' to ',rightr:t:t,' = ',roottest:t:t,' with ',stepr,' steps');
      writeln('____________________');
      writeln('Squares:');
      writeln('Chose square mode: 1-square(ftest1),2-square(ftest2),3-square(both),4-another function');
      readln(squaremode);
      if squaremode=1 then 
        begin
          writeln('Input the segment(square(ftest1))');
          read(lefts1,rights1);
          squaretest1:=square(ftest1,lefts1,rights1);
          writeln('Square(ftest1) from ',lefts1:t:t,' to ',rights1:t:t,' = ',squaretest1:t:t,
            ' with ',stepi, ' steps and ',segments, ' segments');
        end;
        if squaremode=2 then 
        begin
          writeln('Input the segment(square(ftest2))');
          read(lefts2,rights2);
          squaretest1:=square(ftest2,lefts2,rights2);
          writeln('Square(ftest2) from ',lefts2:t:t,' to ',rights2:t:t,' = ',squaretest2:t:t,
            ' with ',stepi, ' steps and ',segments, ' segments');
        end;
        if squaremode=4 then 
        begin
          writeln('Input the segment(square(ftest3))');
          read(lefts3,rights3);
          squaretest3:=square(ftest3,lefts3,rights3);
          writeln('Square(ftest3) from ',lefts3:t:t,' to ',rights3:t:t,' = ',squaretest3:t:t,
            ' with ',stepi, ' steps and ',segments, ' segments');
        end;
        if squaremode=3 then 
        begin
          writeln('Input the segment(square(ftest1))');
          read(lefts1,rights1);
          squaretest1:=square(ftest1,lefts1,rights1);
          writeln('Square(ftest1) from ',lefts1:t:t,' to ',rights1:t:t,' = ',squaretest1:t:t,
            ' with ',stepi, ' steps and ',segments, ' segments');
          writeln('____________________');
          stepi:=1;
          segments:=1;
          writeln('Input the segment(square(ftest2))');
          read(lefts2,rights2);
          squaretest2:=square(ftest2,lefts2,rights2);
          writeln('Square(ftest2) from ',lefts2:t:t,' to ',rights2:t:t,' = ',squaretest2:t:t,
            ' with ',stepi, ' steps and ',segments, ' segments');
        end;
    end;
end.
    
  
  