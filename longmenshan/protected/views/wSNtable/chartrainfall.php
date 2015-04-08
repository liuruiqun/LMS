<?php
/**
 * Created by PhpStorm.
 * User: LiuRuiQun
 * Date: 15-1-28
 * Time: 下午4:06
 */
/*
$this->Widget('HighchartsWidget', array(
    'scripts' => array(
        'modules/exporting',
        'themes/grid-light',
    ),
    'options' => array(
        'title' => array(
            'text' => 'Combination chart',
        ),
        'xAxis' => array(
            'categories' => array('Apples', 'Oranges', 'Pears', 'Bananas', 'Plums'),
        ),
        'labels' => array(
            'items' => array(
                array(
                    'html' => 'Total fruit consumption',
                    'style' => array(
                        'left' => '50px',
                        'top' => '18px',
                        'color' => 'js:(Highcharts.theme && Highcharts.theme.textColor) || \'black\'',
                    ),
                ),
            ),
        ),
        'series' => array(
            array(
                'type' => 'column',
                'name' => 'Jane',
                'data' => array(3, 2, 1, 3, 4),
            ),
            array(
                'type' => 'column',
                'name' => 'John',
                'data' => array(2, 3, 5, 7, 6),
            ),
            array(
                'type' => 'column',
                'name' => 'Joe',
                'data' => array(4, 3, 3, 9, 0),
            ),
            array(
                'type' => 'spline',
                'name' => 'Average',
                'data' => array(3, 2.67, 3, 6.33, 3.33),
                'marker' => array(
                    'lineWidth' => 2,
                    'lineColor' => 'js:Highcharts.getOptions().colors[3]',
                    'fillColor' => 'white',
                ),
            ),
            array(
                'type' => 'pie',
                'name' => 'Total consumption',
                'data' => array(
                    array(
                        'name' => 'Jane',
                        'y' => 13,
                        'color' => 'js:Highcharts.getOptions().colors[0]', // Jane's color
                    ),
                    array(
                        'name' => 'John',
                        'y' => 23,
                        'color' => 'js:Highcharts.getOptions().colors[1]', // John's color
                    ),
                    array(
                        'name' => 'Joe',
                        'y' => 19,
                        'color' => 'js:Highcharts.getOptions().colors[2]', // Joe's color
                    ),
                ),
                'center' => array(100, 80),
                'size' => 100,
                'showInLegend' => false,
                'dataLabels' => array(
                    'enabled' => false,
                ),
            ),
        ),
    )
));
?>

<?php
*/

foreach($dataProvider as $k=>$v) {
    $categories[] = $k."月份";
    $counts[] = $v;
}

$this->Widget('HighchartsWidget', array(
    'scripts' => array(
        'modules/exporting',
        'themes/grid-light',
    ),
    'options' => array(
        'title' => array('text' => '2014年9月份到2015年3月份 雨量'),
        'xAxis' => array(
            //'categories' => $categories,
            'categories' => array(9,10,11,12,1,2,3),
        ),
        'yAxis' => array(
            'title' => array('text' => '雨量 （单位：毫米）')
        ),
        'labels' => array(
            'items' => array(
                array(
                    //'html' => '2014年9月份到2015年3月份降雨统计',
                    'style' => array(
                        'left' => '50px',
                        'top' => '18px',
                        'color' => 'js:(Highcharts.theme && Highcharts.theme.textColor) || \'black\'',
                    ),
                ),
            ),
        ),
        'series' => array(
            array(
                'type' => 'column',
                'name' => '2014年9月份到2015年3月份 雨量',
                'data' => $counts,
            ),
            array(
                'type' => 'spline',
                'name' => '雨量',
                'data' => $counts,
                'marker' => array(
                    'lineWidth' => 2,
                    'lineColor' => 'js:Highcharts.getOptions().colors[3]',
                    'fillColor' => 'white',
                ),
            ),
//            array(
//                'type' => 'pie',
//                'name' => '2014年1月份到12月份 雨量',
//                'data' => $counts,
//                'center' => array(100, 80),
//                'size' => 100,
//                'showInLegend' => false,
//                'dataLabels' => array(
//                    'enabled' => false,
//                ),
//            ),
        ),

    )
));?>