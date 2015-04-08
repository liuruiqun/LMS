<?php
/**
 * Created by PhpStorm.
 * User: LiuRuiQun
 * Date: 15-1-28
 * Time: 下午3:50
 */

$this->breadcrumbs=array(
    '震动',
);

$this->menu=array(
    array('label'=>'Create rainfalldata', 'url'=>array('create')),
    array('label'=>'Manage rainfalldata', 'url'=>array('admin')),
);
?>


<?php $this->widget('zii.widgets.CListView', array(
    'dataProvider'=>$dataProvider,
    'itemView'=>'_view',
)); ?>
