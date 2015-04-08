<?php
/**
 * Created by PhpStorm.
 * User: LiuRuiQun
 * Date: 15-1-28
 * Time: 下午3:36
 */
$this->breadcrumbs=array(
    '雨量',
);

$this->menu=array(
    array('label'=>'Create rainfalldata', 'url'=>array('create')),
    array('label'=>'Manage rainfalldata', 'url'=>array('admin')),
    array('label'=>'Chart rainfalldata', 'url'=>array('chartrainfall')),

);
?>

<h1雨量</h1>

<?php $this->widget('zii.widgets.CListView', array(
    'dataProvider'=>$dataProvider,
    'itemView'=>'_view',
)); ?>
