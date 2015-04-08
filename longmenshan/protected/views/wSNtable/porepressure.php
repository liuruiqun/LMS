<?php
/**
 * Created by PhpStorm.
 * User: LiuRuiQun
 * Date: 15-1-29
 * Time: 下午2:04
 */
$this->breadcrumbs=array(
    '孔压',
);

$this->menu=array(
    array('label'=>'Create rainfalldata', 'url'=>array('create')),
    array('label'=>'Manage rainfalldata', 'url'=>array('admin')),
    array('label'=>'Chart rainfalldata', 'url'=>array('chartrainfall')),

);
?>


<?php $this->widget('zii.widgets.CListView', array(
    'dataProvider'=>$dataProvider,
    'itemView'=>'_view',
)); ?>
